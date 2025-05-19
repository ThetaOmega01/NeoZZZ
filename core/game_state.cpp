#include "game_state.hpp"
#include "move.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace tetris {

GameState::GameState(const int32_t width, const int32_t height)
    : m_board{width, height} {}

GameState::GameState(const int32_t width, const int32_t height,
                     std::shared_ptr<RotationSystem> rotationSystem)
    : m_board{width, height}, m_rotationSystem{std::move(rotationSystem)} {
  // If we have a rotation system, make sure the current piece knows about it
  if (m_rotationSystem) {
    m_currentPiece.setRotationSystem(m_rotationSystem);
  }
}

bool GameState::applyMove(const Move& move) {
  if (m_gameOver) {
    return false;
  }

  PieceState newState{m_currentPiece.getState()};
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
    // Apply wall kick if needed
    if (move.getWallKickIndex() >= 0 && m_rotationSystem) {
      // Get the wall kick data for this rotation
      const WallKickData& wallKicks = m_rotationSystem->getClockwiseWallKicks(
          newState.getType(), rotateCounterClockwise(newState.getRotation()));

      // Apply the wall kick offset at the specified index
      if (move.getWallKickIndex() <
          static_cast<int32_t>(wallKicks.getTestCount())) {
        const WallKickOffset& offset =
            wallKicks.getOffset(move.getWallKickIndex());
        newPos.xPos += offset.xOffset;
        newPos.yPos += offset.yOffset;
      }
    }
    break;
  case MoveType::RotateCounterClockwise:
    newState.setRotation(rotateCounterClockwise(newState.getRotation()));
    // Apply wall kick if needed
    if (move.getWallKickIndex() >= 0 && m_rotationSystem) {
      // Get the wall kick data for this rotation
      const WallKickData& wallKicks =
          m_rotationSystem->getCounterClockwiseWallKicks(
              newState.getType(), rotateClockwise(newState.getRotation()));

      // Apply the wall kick offset at the specified index
      if (move.getWallKickIndex() <
          static_cast<int32_t>(wallKicks.getTestCount())) {
        const WallKickOffset& offset =
            wallKicks.getOffset(move.getWallKickIndex());
        newPos.xPos += offset.xOffset;
        newPos.yPos += offset.yOffset;
      }
    }
    break;
  case MoveType::Rotate180:
    newState.setRotation(rotate180(newState.getRotation()));
    // Apply wall kick if needed
    if (move.getWallKickIndex() >= 0 && m_rotationSystem) {
      // Get the wall kick data for this rotation
      const WallKickData& wallKicks = m_rotationSystem->get180WallKicks(
          newState.getType(), rotate180(newState.getRotation()));

      // Apply the wall kick offset at the specified index
      if (move.getWallKickIndex() <
          static_cast<int32_t>(wallKicks.getTestCount())) {
        const WallKickOffset& offset =
            wallKicks.getOffset(move.getWallKickIndex());
        newPos.xPos += offset.xOffset;
        newPos.yPos += offset.yOffset;
      }
    }
    break;
  case MoveType::HardDrop: {
    // Move the piece down until it collides
    while (!checkCollision(newState, Position(newPos.xPos, newPos.yPos - 1))) {
      newPos.yPos -= 1;
    }
    break;
  }
  case MoveType::SoftDrop:
    // Same as down, but might have different scoring in a real game
    newPos.yPos -= 1;
    break;
  case MoveType::Hold:
    if (m_holdUsed) {
      return false;
    }
    return holdCurrentPiece();
  }

  // Update the position in the new state
  newState.setPosition(newPos);

  // Check if the new state is valid
  if (!isValidState(newState)) {
    return false;
  }

  // Apply the new state
  m_currentPiece.setState(newState);
  return true;
}

bool GameState::isValidState(const PieceState& state) const {
  // Create a temporary piece with the new state
  const Piece tempPiece(state, m_rotationSystem);

  // Check if any of the piece's cells are out of bounds or collide with the
  // board
  return std::ranges::all_of(
      tempPiece.getAbsoluteFilledCells(), [this](const auto& cell) {
        const auto& [xPos, yPos] = cell;
        return xPos >= 0 && xPos < m_board.getWidth() && yPos >= 0 &&
               yPos < m_board.getHeight() && !m_board.isFilled(xPos, yPos);
      });
}

bool GameState::checkCollision(const PieceState& state,
                               const Position& position) const {
  // Create a temporary state with the given position
  PieceState tempState = state;
  tempState.setPosition(position);

  return !isValidState(tempState);
}

int32_t GameState::lockCurrentPiece() {
  // Add the piece to the board
  for (const auto& [xPos, yPos] : m_currentPiece.getAbsoluteFilledCells()) {
    m_board.fillCell(xPos, yPos);
  }

  // Clear any filled rows
  const int32_t linesCleared{m_board.clearFilledRows()};
  m_linesCleared += linesCleared;

  // Reset hold usage
  m_holdUsed = false;

  return linesCleared;
}

bool GameState::spawnPiece(const PieceType type) {
  if (!m_rotationSystem) {
    throw std::runtime_error("Rotation system not set");
  }

  // Use the rotation system to get the initial state
  const PieceState state{m_rotationSystem->getInitialState(
      type, m_board.getWidth(), m_board.getHeight())};

  // Create a new piece with the state and rotation system
  m_currentPiece = Piece(state, m_rotationSystem);

  // Check if the piece can be placed without collisions
  if (!isValidState(state)) {
    // Collision detected, game over
    m_gameOver = true;
    return false;
  }

  return true;
}

bool GameState::spawnNextPiece() {
  if (m_nextPieces.empty()) {
    return false;
  }

  const PieceType nextType{m_nextPieces.front()};
  m_nextPieces.pop_front();

  return spawnPiece(nextType);
}

bool GameState::holdCurrentPiece() {
  if (m_holdUsed) {
    return false;
  }

  PieceType currentType{m_currentPiece.getState().getType()};

  if (m_heldPiece.has_value()) {
    // Swap current piece with the held piece
    PieceType heldType{m_heldPiece.value()};
    m_heldPiece = currentType;

    // Spawn the held piece
    if (!spawnPiece(heldType)) {
      // If spawning fails, restore the held piece and return false
      m_heldPiece = heldType;
      return false;
    }
  } else {
    // No held piece, so just store the current piece
    m_heldPiece = currentType;

    // Spawn the next piece
    if (!spawnNextPiece()) {
      // If spawning fails, restore the held piece and return false
      m_heldPiece.reset();
      return false;
    }
  }

  m_holdUsed = true;
  return true;
}

GameState GameState::clone() const {
  GameState copy{m_board.getWidth(), m_board.getHeight()};
  copy.m_board = m_board;
  copy.m_rotationSystem = m_rotationSystem;
  copy.m_currentPiece = m_currentPiece;
  copy.m_heldPiece = m_heldPiece;
  copy.m_holdUsed = m_holdUsed;
  copy.m_nextPieces = m_nextPieces;
  copy.m_linesCleared = m_linesCleared;
  copy.m_gameOver = m_gameOver;

  return copy;
}

std::string GameState::toString() const {
  std::ostringstream oss;

  oss << "Game State:\n";
  oss << "  Board: " << m_board.getWidth() << "x" << m_board.getHeight()
      << "\n";
  oss << "  Current Piece: "
      << static_cast<char>(m_currentPiece.getState().getType()) << "\n";
  oss << "  Held Piece: "
      << (m_heldPiece.has_value()
              ? std::string(1, static_cast<char>(m_heldPiece.value()))
              : "None")
      << "\n";
  oss << "  Hold Used: " << (m_holdUsed ? "Yes" : "No") << "\n";
  oss << "  Next Pieces: ";
  for (const auto& piece : m_nextPieces) {
    oss << static_cast<char>(piece) << " ";
  }
  oss << "\n";
  oss << "  Lines Cleared: " << m_linesCleared << "\n";
  oss << "  Game Over: " << (m_gameOver ? "Yes" : "No") << "\n";

  return oss.str();
}

bool GameState::checkCollision() const {
  return std::ranges::any_of(
      m_currentPiece.getAbsoluteFilledCells(), [this](const auto& cell) {
        return cell.xPos < 0 || cell.xPos >= m_board.getWidth() ||
               cell.yPos < 0 || cell.yPos >= m_board.getHeight() ||
               m_board.isFilled(cell.xPos, cell.yPos);
      });
}

} // namespace tetris
