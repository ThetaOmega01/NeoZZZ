#include "path_search.hpp"
#include "search_algorithm.hpp"
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <vector>

namespace tetris {

PathSearch::PathSearch(const Config& config) { PathSearch::initialize(config); }

void PathSearch::initialize(const Config& config) { m_config = config; }

std::vector<LandingPosition>
PathSearch::findLandingPositions(const GameState& gameState, const Piece& piece,
                                 size_t maxDepth) const {
  std::vector<LandingPosition> landingPositions{};

  // Use a queue for BFS
  std::queue<std::shared_ptr<SearchNode>> queue{};

  // Use an unordered_set to track visited states
  std::unordered_set<PieceState, PieceStateHash, PieceStateEqual> visited{};

  // Start with the initial piece
  auto startNode{
      std::make_shared<SearchNode>(piece, Move{MoveType::Down}, nullptr)};
  queue.push(startNode);
  visited.insert(piece.getState());

  // BFS to find all reachable landing positions
  while (!queue.empty()) {
    auto currentNode{queue.front()};
    queue.pop();

    // Check if we've reached the maximum depth
    if (maxDepth > 0) {
      // Count the depth by traversing up the parent chain
      size_t depth{0};
      auto node{currentNode};
      while (node->parent != nullptr) {
        ++depth;
        node = node->parent;
      }

      if (depth >= maxDepth) {
        continue;
      }
    }

    // Check if we've reached a landing position
    if (isAtLandingPosition(gameState, currentNode->piece)) {
      // Create a landing position
      LandingPosition landingPos{currentNode->piece};

      // Reconstruct the path
      std::vector<Move> path{reconstructPath(currentNode)};
      landingPos.setPath(path);

      // Check for T-spins if this is a T piece
      if (currentNode->piece.getState().getType() == PieceType::T) {
        bool lastMoveWasRotation{
            !path.empty() &&
            (path.back().getType() == MoveType::RotateClockwise ||
             path.back().getType() == MoveType::RotateCounterClockwise ||
             path.back().getType() == MoveType::Rotate180)};
        const auto& gameBoard{gameState.getBoard()};

        auto isCellOccupied_lambda_opt{
            [&gameBoard](int32_t xPos, int32_t yPos) -> bool {
              if (xPos < 0 || xPos >= gameBoard.getWidth() || yPos < 0 ||
                  yPos >= gameBoard.getHeight()) {
                return true; // Out of bounds is considered occupied
              }
              return gameBoard.isFilled(xPos, yPos);
            }};

        auto detectTSpin_lambda_opt{[&isCellOccupied_lambda_opt](
                                        const Piece& tPiece,
                                        bool lastMoveWasRotation) -> int32_t {
          if (!lastMoveWasRotation) {
            return 0; // Type 0: None
          }

          const PieceState& tState{tPiece.getState()};
          int32_t pivotX{tState.getPosition().xPos};
          int32_t pivotY{tState.getPosition().yPos};

          std::array<Position, 4> corners{{
              {pivotX - 1, pivotY + 1}, // A (top-left)
              {pivotX + 1, pivotY + 1}, // B (top-right)
              {pivotX - 1, pivotY - 1}, // C (bottom-left)
              {pivotX + 1, pivotY - 1}  // D (bottom-right)
          }};

          std::array<bool, 4> filledCorners{
              isCellOccupied_lambda_opt(corners.at(0).xPos,
                                        corners.at(0).yPos), // A
              isCellOccupied_lambda_opt(corners.at(1).xPos,
                                        corners.at(1).yPos), // B
              isCellOccupied_lambda_opt(corners.at(2).xPos,
                                        corners.at(2).yPos), // C
              isCellOccupied_lambda_opt(corners.at(3).xPos,
                                        corners.at(3).yPos) // D
          };
          int32_t tSpinType{0};
          int32_t occupiedCorners{
              std::accumulate(filledCorners.begin(), filledCorners.end(), 0)};
          if (occupiedCorners >= 3) {
            tSpinType = 1; // Full T-Spin
          } else if (occupiedCorners == 2) {
            int32_t rotation{static_cast<int32_t>(tState.getRotation())};
            bool miniTspin{false};
            switch (rotation) {
            case 0:
              miniTspin = filledCorners.at(0) && filledCorners.at(1); // A & B
              break;
            case 1:
              miniTspin = filledCorners.at(1) && filledCorners.at(3); // B & D
              break;
            case 2:
              miniTspin = filledCorners.at(2) && filledCorners.at(3); // C & D
              break;
            case 3:
              miniTspin = filledCorners.at(0) && filledCorners.at(2); // D & C
              break;
            default:
              miniTspin = false;
              break;
            }
            if (miniTspin) {
              tSpinType = 2; // Mini T-Spin
            }
          }
          return tSpinType;
        }};

        int32_t tSpinType{
            detectTSpin_lambda_opt(currentNode->piece, lastMoveWasRotation)};
        landingPos.setTSpinType(tSpinType);
      } else { // Not a T-piece
        landingPos.setTSpinType(0);
      }

      // Add to the landing positions
      landingPositions.push_back(landingPos);
    }

    // Try all possible moves
    std::vector<Move> possibleMoves{};

    // Add translation moves
    possibleMoves.emplace_back(MoveType::Left);
    possibleMoves.emplace_back(MoveType::Right);

    if (m_config.allowSoftDrop) {
      possibleMoves.emplace_back(MoveType::Down);
    }

    if (m_config.allowHardDrop) {
      possibleMoves.emplace_back(MoveType::HardDrop);
    }

    // Add rotation moves
    possibleMoves.emplace_back(MoveType::RotateClockwise);
    possibleMoves.emplace_back(MoveType::RotateCounterClockwise);

    if (m_config.allowRotate180) {
      possibleMoves.emplace_back(MoveType::Rotate180);
    }

    // Try each move
    for (const auto& move : possibleMoves) {
      if (isValidMove(gameState, currentNode->piece, move)) {
        Piece newPiece{applyMove(gameState, currentNode->piece, move)};

        // Check if we've already visited this state
        if (!visited.contains(newPiece.getState())) {
          // Add to the queue
          auto newNode{
              std::make_shared<SearchNode>(newPiece, move, currentNode)};
          queue.push(newNode);
          visited.insert(newPiece.getState());
        }
      }
    }
  }

  return landingPositions;
}

std::vector<Move> PathSearch::findPath(const GameState& gameState,
                                       const Piece& startPiece,
                                       const Piece& targetPiece) const {
  // Use a queue for BFS
  std::queue<std::shared_ptr<SearchNode>> queue{};

  // Use an unordered_set to track visited states
  std::unordered_set<PieceState, PieceStateHash, PieceStateEqual> visited{};

  // Start with the initial piece
  auto startNode{
      std::make_shared<SearchNode>(startPiece, Move{MoveType::Down}, nullptr)};
  queue.push(startNode);
  visited.insert(startPiece.getState());

  // Target state
  const PieceState& targetState{targetPiece.getState()};

  // BFS to find the path
  while (!queue.empty()) {
    auto currentNode{queue.front()};
    queue.pop();

    // Check if we've reached the target state
    if (currentNode->piece.getState() == targetState) {
      // Reconstruct the path
      return reconstructPath(currentNode);
    }

    // Try all possible moves
    std::vector<Move> possibleMoves{};

    // Add translation moves
    possibleMoves.emplace_back(MoveType::Left);
    possibleMoves.emplace_back(MoveType::Right);

    if (m_config.allowSoftDrop) {
      possibleMoves.emplace_back(MoveType::Down);
    }

    if (m_config.allowHardDrop) {
      possibleMoves.emplace_back(MoveType::HardDrop);
    }

    // Add rotation moves
    possibleMoves.emplace_back(MoveType::RotateClockwise);
    possibleMoves.emplace_back(MoveType::RotateCounterClockwise);

    if (m_config.allowRotate180) {
      possibleMoves.emplace_back(MoveType::Rotate180);
    }

    // Try each move
    for (const auto& move : possibleMoves) {
      if (isValidMove(gameState, currentNode->piece, move)) {
        Piece newPiece{applyMove(gameState, currentNode->piece, move)};

        // Check if we've already visited this state
        if (!visited.contains(newPiece.getState())) {
          // Add to the queue
          auto newNode{
              std::make_shared<SearchNode>(newPiece, move, currentNode)};
          queue.push(newNode);
          visited.insert(newPiece.getState());
        }
      }
    }
  }

  // If we reach here, no path was found
  return std::vector<Move>{};
}

bool PathSearch::canPlacePiece(const GameState& gameState,
                               const Piece& piece) const {
  // Check if all the piece's cells are within bounds and not colliding with the
  // board
  return std::ranges::all_of(
      piece.getAbsoluteFilledCells(), [&gameState](const auto& cell) {
        const auto& [xPos, yPos] = cell;
        return xPos >= 0 && xPos < gameState.getBoard().getWidth() &&
               yPos >= 0 && yPos < gameState.getBoard().getHeight() &&
               !gameState.getBoard().isFilled(xPos, yPos);
      });
}

std::vector<Move>
PathSearch::reconstructPath(std::shared_ptr<SearchNode>& node) {
  std::vector<Move> path{};

  // Traverse up the parent chain to reconstruct the path
  while (node->parent != nullptr) {
    path.push_back(node->lastMove);
    node = node->parent;
  }

  std::ranges::reverse(path);
  return path;
}

bool PathSearch::isValidMove(const GameState& gameState, const Piece& piece,
                             const Move& move) const {
  // Apply the move to the piece
  Piece newPiece{applyMove(gameState, piece, move)};

  // Check if the new piece can be placed on the board
  return canPlacePiece(gameState, newPiece);
}

Piece PathSearch::applyMove(const GameState& gameState, const Piece& piece,
                            const Move& move) const {
  Piece newPiece{piece};
  PieceState newState{piece.getState()};
  Position newPos{newState.getPosition()};

  switch (move.getType()) {
  case MoveType::Left:
    newPos.xPos -= 1;
    break;
  case MoveType::Right:
    newPos.xPos += 1;
    break;
  case MoveType::Down:
    newPos.yPos -= 1;
    break;
  case MoveType::Up:
    newPos.yPos += 1;
    break;
  case MoveType::RotateClockwise:
    newState.setRotation(rotateClockwise(newState.getRotation()));
    break;

  case MoveType::RotateCounterClockwise:
    newState.setRotation(rotateCounterClockwise(newState.getRotation()));
    break;

  case MoveType::Rotate180:
    newState.setRotation(rotate180(newState.getRotation()));
    break;
  case MoveType::HardDrop:
    // Move the piece down until it collides
    while (true) {
      Position testPos = newPos;
      testPos.yPos -= 1;

      PieceState testState = newState;
      testState.setPosition(testPos);

      Piece testPiece = newPiece;
      testPiece.setState(testState);

      if (!canPlacePiece(gameState, testPiece)) {
        break;
      }

      newPos = testPos;
    }
    break;

  default:
    // Unsupported move type
    break;
  }

  newState.setPosition(newPos);
  newPiece.setState(newState);
  return newPiece;
}

bool PathSearch::isAtLandingPosition(const GameState& gameState,
                                     const Piece& piece) const {
  // A piece is at a landing position if it can't move down any further
  Position newPos = piece.getState().getPosition();
  newPos.yPos -= 1;

  PieceState newState = piece.getState();
  newState.setPosition(newPos);

  Piece newPiece = piece;
  newPiece.setState(newState);

  return !canPlacePiece(gameState, newPiece);
}

} // namespace tetris
