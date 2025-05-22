#pragma once

#include "path_search.hpp"
#include "search_algorithm.hpp"

#include <string_view>

using namespace std::string_view_literals;

namespace tetris {

/**
 * @brief Enumeration of T-spin types
 */
enum class TSpinType {
  None,    ///< Not a T-spin
  Regular, ///< Regular T-spin
  Mini     ///< Mini T-spin
};

/**
 * @class TSpinSearch
 * @brief Specialized search algorithm for finding T-spin opportunities
 *
 * This class implements a search algorithm specifically designed to
 * find T-spin opportunities on the board.
 */

class TSpinSearch : public SearchAlgorithm {
public:
  /**
   * @brief Extended configuration for T-spin search
   */
  struct TSpinConfig : public Config {
    bool requireLastRotation{true}; ///< Require the last move to be a rotation
    bool allowMiniTSpins{true};     ///< Allow mini T-spins
    bool prioritizeTSpins{true}; ///< Prioritize T-spins over other placements
  };

  /**
   * @brief Default constructor
   */
  TSpinSearch();

  /**
   * @brief Construct with configuration
   *
   * @param config The search configuration
   */
  explicit TSpinSearch(const TSpinConfig& config);

  /**
   * @brief Get the name of the search algorithm
   *
   * @return The name "TSpinSearch"
   */
  [[nodiscard]] std::string_view getName() const override {
    return "TSpinSearch"sv;
  }

  /**
   * @brief Initialize the search algorithm
   *
   * @param config Configuration options
   */
  void initialize(const Config& config) override;

  /**
   * @brief Initialize with T-spin specific configuration
   *
   * @param config T-spin specific configuration
   */
  void initialize(const TSpinConfig& config);

  /**
   * @brief Find all possible landing positions for a piece
   *
   * @param gameState The current game state
   * @param piece The piece to place
   * @param maxDepth Maximum search depth (0 for unlimited)
   * @return Vector of landing positions
   */
  [[nodiscard]] std::vector<LandingPosition>
  findLandingPositions(const GameState& gameState, const Piece& piece,
                       size_t maxDepth) const override;

  /**
   * @brief Find the path of moves to reach a landing position
   *
   * @param gameState The current game state
   * @param startPiece The starting piece
   * @param targetPiece The target piece position
   * @return Vector of moves to reach the target, empty if no path found
   */
  [[nodiscard]] std::vector<Move>
  findPath(const GameState& gameState, const Piece& startPiece,
           const Piece& targetPiece) const override;

  /**
   * @brief Check if a piece can be placed at the given position
   *
   * @param gameState The current game state
   * @param piece The piece to check
   * @return true if the piece can be placed, false otherwise
   */
  [[nodiscard]] bool canPlacePiece(const GameState& gameState,
                                   const Piece& piece) const override;

  /**
   * @brief Detect if a placement results in a T-spin
   *
   * @param gameState The current game state
   * @param piece The piece to check
   * @param lastMove The last move applied to reach this position
   * @return The type of T-spin (None, Regular, Mini)
   */
  [[nodiscard]] TSpinType detectTSpin(const GameState& gameState,
                                      const Piece& piece,
                                      const Move& lastMove) const;

private:
  /**
   * @brief Check if a T piece is in a T-spin position
   *
   * @param gameState The current game state
   * @param piece The T piece to check
   * @return true if the piece is in a T-spin position, false otherwise
   */
  [[nodiscard]] bool isTSpinPosition(const GameState& gameState,
                                     const Piece& piece) const;

  /**
   * @brief Check if a T piece is in a mini T-spin position
   *
   * @param gameState The current game state
   * @param piece The T piece to check
   * @return true if the piece is in a mini T-spin position, false otherwise
   */
  [[nodiscard]] bool isMiniTSpinPosition(const GameState& gameState,
                                         const Piece& piece) const;

  /**
   * @brief Count the number of filled cells around a T piece
   *
   * @param gameState The current game state
   * @param piece The T piece to check
   * @return The number of filled cells around the T piece
   */
  [[nodiscard]] int countFilledCellsAroundT(const GameState& gameState,
                                            const Piece& piece) const;

  /**
   * @brief Check if a T piece can rotate in any direction
   *
   * @param gameState The current game state
   * @param piece The T piece to check
   * @return true if the piece can rotate, false otherwise
   */
  [[nodiscard]] bool canRotate(const GameState& gameState,
                               const Piece& piece) const;

  /**
   * @brief Detect if a T piece is in a recognized T-slot pattern
   *
   * @param gameState The current game state
   * @param piece The T piece to check
   * @return true if the piece is in a T-slot pattern, false otherwise
   */
  [[nodiscard]] bool detectTSlot(const GameState& gameState,
                                 const Piece& piece) const;

  /**
   * @brief Detect a T2 slot pattern
   *
   * @param gameState The current game state
   * @param pos The position to check
   * @param rotation The rotation to check
   * @return true if a T2 slot is detected, false otherwise
   */
  [[nodiscard]] bool detectT2Slot(const GameState& gameState,
                                  const Position& pos, Rotation rotation) const;

  /**
   * @brief Detect a T3 slot pattern
   *
   * @param gameState The current game state
   * @param pos The position to check
   * @param rotation The rotation to check
   * @return true if a T3 slot is detected, false otherwise
   */
  [[nodiscard]] bool detectT3Slot(const GameState& gameState,
                                  const Position& pos, Rotation rotation) const;

  TSpinConfig m_tSpinConfig; ///< T-spin specific configuration
  PathSearch m_pathSearch;   ///< Path search algorithm
};

} // namespace tetris