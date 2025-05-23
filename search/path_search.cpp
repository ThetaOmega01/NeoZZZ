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
      std::make_shared<SearchNode>(piece, Move{MoveType::Down}, nullptr, 0)};
  queue.push(startNode);
  visited.insert(piece.getState());

  // BFS to find all reachable landing positions
  while (!queue.empty()) {
    auto currentNode{queue.front()};
    queue.pop();

    // Check if we've reached the maximum depth
    if (maxDepth > 0 && currentNode->depth >= maxDepth) {
      continue;
    }

    // Check if we've reached a landing position
    if (isAtLandingPosition(gameState, currentNode->piece)) {
      // Create a landing position
      LandingPosition landingPos{currentNode->piece};

      // Reconstruct the path
      std::vector path{reconstructPath(currentNode)};
      landingPos.setPath(path);

      // Check for T-spins
      bool lastMoveWasRotation{
          !path.empty() &&
          (path.back().getType() == MoveType::RotateClockwise ||
           path.back().getType() == MoveType::RotateCounterClockwise ||
           path.back().getType() == MoveType::Rotate180)};

      // Use the dedicated T-spin detection method (handles T-piece check internally)
      int32_t tSpinType{detectTSpin(gameState, currentNode->piece, lastMoveWasRotation)};
      landingPos.setTSpinType(tSpinType);

      // Add to the landing positions
      landingPositions.push_back(landingPos);
    }

    // Get all possible moves based on configuration

    // Try each move
    for (std::vector<Move> possibleMoves = generatePossibleMoves();
         const auto& move : possibleMoves) {
      if (isValidMove(gameState, currentNode->piece, move)) {

        // Check if we've already visited this state
        if (Piece newPiece{applyMove(gameState, currentNode->piece, move)};
            !visited.contains(newPiece.getState())) {
          // Add to the queue with incremented depth
          auto newNode{
              std::make_shared<SearchNode>(std::move(newPiece), move, currentNode, currentNode->depth + 1)};
          queue.push(newNode);
          visited.insert(newNode->piece.getState());
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
  const auto startNode{
      std::make_shared<SearchNode>(startPiece, Move{MoveType::Down}, nullptr, 0)};
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

    // Get all possible moves based on configuration

    // Try each move
    for (std::vector<Move> possibleMoves = generatePossibleMoves();
         const auto& move : possibleMoves) {
      if (isValidMove(gameState, currentNode->piece, move)) {

        // Check if we've already visited this state
        if (Piece newPiece{applyMove(gameState, currentNode->piece, move)};
            !visited.contains(newPiece.getState())) {
          // Add to the queue with incremented depth
          auto newNode{
              std::make_shared<SearchNode>(std::move(newPiece), move, currentNode, currentNode->depth + 1)};
          queue.push(newNode);
          visited.insert(newNode->piece.getState());
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
  const Piece newPiece{applyMove(gameState, piece, move)};

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
    return applyHardDrop(gameState, piece);

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

std::vector<Move> PathSearch::generatePossibleMoves() const {
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

  return possibleMoves;
}

Piece PathSearch::applyHardDrop(const GameState& gameState, const Piece& piece) const {
  Piece newPiece{piece};
  PieceState newState{piece.getState()};
  Position newPos{newState.getPosition()};

  // Binary search approach for hard drop
  int32_t low{0};
  int32_t high{gameState.getBoard().getHeight()};

  while (low < high) {
    const int32_t mid {low + (high - low) / 2};

    Position testPos{ newPos};
    testPos.yPos -= mid;

    PieceState testState{newState};
    testState.setPosition(testPos);

    Piece testPiece{newPiece};
    testPiece.setState(testState);

    if (canPlacePiece(gameState, testPiece)) {
      low = mid + 1;
    } else {
      high = mid;
    }
  }

  // Move down by (low - 1) steps (or 0 if low is 0)
  if (low > 0) {
    newPos.yPos -= low - 1;
  }

  newState.setPosition(newPos);
  newPiece.setState(newState);
  return newPiece;
}

int32_t PathSearch::detectTSpin(const GameState& gameState,
                               const Piece& piece,
                                const bool lastMoveWasRotation) {
  // Constants for T-spin types
  static constexpr int32_t noTspinType{0};
  static constexpr int32_t regularTspinType{1};
  static constexpr int32_t miniTspinType{2};

  // Early return if not a T piece or last move wasn't rotation
  if (piece.getState().getType() != PieceType::T || !lastMoveWasRotation) {
    return noTspinType;
  }

  const auto& gameBoard{gameState.getBoard()};

  // Helper function to check if a cell is occupied
  auto isCellOccupied = [&gameBoard](const int32_t xPos,
                                     const int32_t yPos) -> bool {
    if (xPos < 0 || xPos >= gameBoard.getWidth() || yPos < 0 ||
        yPos >= gameBoard.getHeight()) {
      return true; // Out of bounds is considered occupied
    }
    return gameBoard.isFilled(xPos, yPos);
  };

  const PieceState& tState{piece.getState()};
  const int32_t pivotX{tState.getPosition().xPos};
  const int32_t pivotY{tState.getPosition().yPos};

  // Define the four corners around the T piece
  const std::array<Position, 4> corners{{
    {pivotX - 1, pivotY + 1}, // A (top-left)
    {pivotX + 1, pivotY + 1}, // B (top-right)
    {pivotX - 1, pivotY - 1}, // C (bottom-left)
    {pivotX + 1, pivotY - 1}  // D (bottom-right)
  }};

  // Check which corners are filled
  std::array filledCorners{
    isCellOccupied(corners.at(0).xPos, corners.at(0).yPos), // A
    isCellOccupied(corners.at(1).xPos, corners.at(1).yPos), // B
    isCellOccupied(corners.at(2).xPos, corners.at(2).yPos), // C
    isCellOccupied(corners.at(3).xPos, corners.at(3).yPos)  // D
  };

  // Count occupied corners
  const int32_t occupiedCorners{
    std::accumulate(filledCorners.begin(), filledCorners.end(), 0)};

  // Determine T-spin type
  if (occupiedCorners >= 3) {
    return regularTspinType; // Full T-Spin
  }
  if (occupiedCorners == 2) {
    const int32_t rotation{static_cast<int32_t>(tState.getRotation())};
    bool miniTspin{false};

    // Check specific corner combinations based on rotation
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
      miniTspin = filledCorners.at(0) && filledCorners.at(2); // A & C
      break;
    default:
      miniTspin = false;
      break;
    }

    if (miniTspin) {
      return miniTspinType; // Mini T-Spin
    }
  }

  return noTspinType;
}

} // namespace tetris
