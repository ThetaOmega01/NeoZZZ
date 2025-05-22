#pragma once

#include "../core/game_state.hpp"
#include "../core/move.hpp"
#include "../core/tetris_piece.hpp"
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

namespace tetris {

/**
 * @brief Represents a landing position for a piece
 */
class LandingPosition {
public:
  /**
   * @brief Default constructor
   */
  LandingPosition() = default;

  /**
   * @brief Construct a landing position with the given piece
   *
   * @param piece The piece at the landing position
   */
  explicit LandingPosition(Piece piece) : m_piece{std::move(piece)} {}

  /**
   * @brief Get the piece at the landing position
   */
  [[nodiscard]] const Piece& getPiece() const { return m_piece; }

  /**
   * @brief Set the piece at the landing position
   */
  void setPiece(const Piece& piece) { m_piece = piece; }

  /**
   * @brief Get the path of moves to reach this landing position
   */
  [[nodiscard]] const std::vector<Move>& getPath() const { return m_path; }

  /**
   * @brief Set the path of moves to reach this landing position
   */
  void setPath(const std::vector<Move>& path) { m_path = path; }

  /**
   * @brief Add a move to the path
   */
  void addMove(const Move& move) { m_path.emplace_back(move); }

  /**
   * @brief Get the T-spin type (if applicable)
   */
  [[nodiscard]] int32_t getTSpinType() const { return m_tSpinType; }

  /**
   * @brief Set the T-spin type
   */
  void setTSpinType(int32_t type) { m_tSpinType = type; }

  /**
   * @brief Check if this landing position results in a T-spin
   */
  [[nodiscard]] bool isTSpin() const { return m_tSpinType > 0; }

  /**
   * @brief Get the number of lines that would be cleared
   */
  [[nodiscard]] int32_t getLinesCleared() const { return m_linesCleared; }

  /**
   * @brief Set the number of lines that would be cleared
   */
  void setLinesCleared(int32_t lines) { m_linesCleared = lines; }

  /**
   * @brief Check if this is a valid landing position
   */
  [[nodiscard]] bool isValid() const { return m_valid; }

  /**
   * @brief Set whether this is a valid landing position
   */
  void setValid(bool valid) { m_valid = valid; }

private:
  Piece m_piece;             ///< The piece at the landing position
  std::vector<Move> m_path;  ///< The path of moves to reach this position
  int32_t m_tSpinType{0};    ///< T-spin type (0=None, 1=T-Spin, 2=T-Spin Mini)
  int32_t m_linesCleared{0}; ///< Number of lines that would be cleared
  bool m_valid{true};        ///< Whether this is a valid landing position
};

/**
 * @class SearchAlgorithm
 * @brief Abstract interface for piece placement search algorithms
 *
 * This class defines the interface for different search algorithms
 * that find possible landing positions for a piece on the board.
 */
class SearchAlgorithm {
public:
  /**
   * @brief Configuration options for search algorithms
   */
  struct Config {
    bool allowRotate180{false}; ///< Allow 180-degree rotations
    bool allowHardDrop{true};   ///< Allow hard drops
    bool allowSoftDrop{true};   ///< Allow soft drops
    bool is20G{false};          ///< Use 20G gravity
    bool lastRotationOnly{
        false}; ///< Only consider positions with rotation as the last move
  };

  /**
   * @brief Virtual destructor
   */
  virtual ~SearchAlgorithm() = default;

  /**
   * @brief Get the name of the search algorithm
   *
   * @return The name of the search algorithm
   */
  [[nodiscard]] virtual std::string_view getName() const = 0;

  /**
   * @brief Initialize the search algorithm
   *
   * @param config Configuration options
   */
  virtual void initialize(const Config& config) = 0;

  /**
   * @brief Find all possible landing positions for a piece
   *
   * @param gameState The current game state
   * @param piece The piece to place
   * @param maxDepth Maximum search depth (0 for unlimited)
   * @return Vector of landing positions
   */
  [[nodiscard]] virtual std::vector<LandingPosition>
  findLandingPositions(const GameState& gameState, const Piece& piece,
                       size_t maxDepth) const = 0;

  /**
   * @brief Find the path of moves to reach a landing position
   *
   * @param gameState The current game state
   * @param startPiece The starting piece
   * @param targetPiece The target piece position
   * @return Vector of moves to reach the target, empty if no path found
   */
  [[nodiscard]] virtual std::vector<Move>
  findPath(const GameState& gameState, const Piece& startPiece,
           const Piece& targetPiece) const = 0;

  /**
   * @brief Check if a piece can be placed at the given position
   *
   * @param gameState The current game state
   * @param piece The piece to check
   * @return true if the piece can be placed, false otherwise
   */
  [[nodiscard]] virtual bool canPlacePiece(const GameState& gameState,
                                           const Piece& piece) const = 0;

  /**
   * @brief Get the configuration options
   */
  [[nodiscard]] const Config& getConfig() const { return m_config; }

  /**
   * @brief Set the configuration options
   */
  void setConfig(const Config& config) { m_config = config; }

protected:
  Config m_config; ///< Configuration options
};

} // namespace tetris