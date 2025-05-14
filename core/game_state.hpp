#pragma once

#include "../rotation_systems/rotation_system.hpp"
#include "tetris_board.hpp"
#include "tetris_piece.hpp"
#include <deque>
#include <optional>

namespace tetris {

/**
 * @brief Represents the current state
 */
class GameState {
public:
  /**
   * @brief Construct a game state with the given board dimensions
   *
   * @param width Width of the board
   * @param height Height of the board
   */
  GameState(std::int32_t width, std::int32_t height);

  /**
   * @brief Construct a game state with the given board dimensions and rotation
   * system
   *
   * @param width Width of the board
   * @param height Height of the board
   * @param rotationSystem The rotation system to use
   */
  GameState(std::int32_t width, std::int32_t height,
            std::shared_ptr<RotationSystem> rotationSystem);

  /**
   * @brief Set the rotation system
   *
   * @param rotationSystem The rotation system to use
   */
  void setRotationSystem(std::shared_ptr<RotationSystem> rotationSystem) {
    m_rotationSystem = std::move(rotationSystem);
    if (m_rotationSystem) {
      m_currentPiece.setRotationSystem(m_rotationSystem);
    }
  }

  /**
   * @brief Get the rotation system
   *
   * @return The rotation system
   */
  [[nodiscard]] std::shared_ptr<RotationSystem> getRotationSystem() const {
    return m_rotationSystem;
  }

  /**
   * @brief Get the game board
   */
  [[nodiscard]] const Board& getBoard() const { return m_board; }

  /**
   * @brief Get a mutable reference to the game board
   */
  Board& getBoard() { return m_board; }

  /**
   * @brief Get the current active piece
   */
  [[nodiscard]] const Piece& getCurrentPiece() const { return m_currentPiece; }

  /**
   * @brief Get a mutable reference to the current active piece
   */
  Piece& getCurrentPiece() { return m_currentPiece; }

  /**
   * @brief Get the held piece type
   */
  [[nodiscard]] std::optional<PieceType> getHeldPiece() const {
    return m_heldPiece;
  }

  /**
   * @brief Set the held piece type
   */
  void setHeldPiece(std::optional<PieceType> type) { m_heldPiece = type; }

  /**
   * @brief Check if hold has been used in the current turn
   */
  [[nodiscard]] bool isHoldUsed() const { return m_holdUsed; }

  /**
   * @brief Set whether hold has been used in the current turn
   */
  void setHoldUsed(bool used) { m_holdUsed = used; }

  /**
   * @brief Get the next pieces in the queue
   */
  [[nodiscard]] const std::deque<PieceType>& getNextPieces() const {
    return m_nextPieces;
  }

  /**
   * @brief Get a mutable reference to the next pieces queue
   */
  std::deque<PieceType>& getNextPieces() { return m_nextPieces; }

  /**
   * @brief Get the number of lines cleared
   */
  [[nodiscard]] std::int32_t getLinesCleared() const { return m_linesCleared; }

  /**
   * @brief Set the number of lines cleared
   */
  void setLinesCleared(std::int32_t lines) { m_linesCleared = lines; }

  /**
   * @brief Check if the game is over
   */
  [[nodiscard]] bool isGameOver() const { return m_gameOver; }

  /**
   * @brief Set whether the game is over
   */
  void setGameOver(bool gameOver) { m_gameOver = gameOver; }

  /**
   * @brief Apply a move to the current piece
   *
   * @param move The move to apply
   * @return true if the move was successful, false otherwise
   */
  bool applyMove(const Move& move);

  /**
   * @brief Lock the current piece into the board
   *
   * @return The number of lines cleared
   */
  std::int32_t lockCurrentPiece();

  /**
   * @brief Spawn a new piece
   *
   * @param type The type of piece to spawn
   * @return true if the piece was spawned successfully, false if it would
   * collide
   */
  bool spawnPiece(PieceType type);

  /**
   * @brief Get the next piece from the queue and spawn it
   *
   * @return true if the piece was spawned successfully, false if it would
   * collide
   */
  bool spawnNextPiece();

  /**
   * @brief Hold the current piece and spawn the held piece or next piece
   *
   * @return true if the operation was successful, false otherwise
   */
  bool holdCurrentPiece();

  /**
   * @brief Create a deep copy of the game state
   *
   * @return A new game state object that is a copy of this one
   */
  [[nodiscard]] GameState clone() const;

  /**
   * @brief Get a string representation of the game state
   *
   * @return A string representing the current game state
   */
  [[nodiscard]] std::string toString() const;

private:
  /**
   * @brief Check if the current piece collides with the board or is out of
   * bounds
   *
   * @return true if there is a collision, false otherwise
   */
  [[nodiscard]] bool checkCollision() const;

private:
  Board m_board;                        ///< The game board
  Piece m_currentPiece;                 ///< The current active piece
  std::optional<PieceType> m_heldPiece; ///< The held piece, if any
  bool m_holdUsed{false}; ///< Whether hold has been used in the current turn
  std::deque<PieceType> m_nextPieces; ///< Queue of upcoming pieces
  std::int32_t m_linesCleared{0};     ///< Total number of lines cleared
  bool m_gameOver{false};             ///< Whether the game is over
  std::shared_ptr<RotationSystem>
      m_rotationSystem; ///< The rotation system to use
};

} // namespace tetris