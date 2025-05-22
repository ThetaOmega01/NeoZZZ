#pragma once

#include "search_algorithm.hpp"
#include <memory>
#include <string_view>
#include <utility>
using namespace std::string_view_literals;

namespace tetris {

/**
 * @class PathSearch
 * @brief Implementation of a breadth-first search algorithm for finding paths
 *
 * This class implements a breadth-first search algorithm to find paths
 * between piece positions on the board.
 */
class PathSearch : public SearchAlgorithm {
public:
  /**
   * @brief Default constructor
   */
  PathSearch() = default;

  /**
   * @brief Construct with configuration
   *
   * @param config The search configuration
   */
  explicit PathSearch(const Config& config);

  /**
   * @brief Get the name of the search algorithm
   *
   * @return The name "PathSearch"
   */
  [[nodiscard]] std::string_view getName() const override {
    return "PathSearch"sv;
  }

  /**
   * @brief Initialize the search algorithm
   *
   * @param config Configuration options
   */
  void initialize(const Config& config) override;

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

private:
  /**
   * @brief Internal structure for BFS search
   *
   * Uses shared_ptr for automatic memory management in the search tree.
   * This prevents memory leaks when nodes are created during the search.
   */
  struct SearchNode {
    Piece piece;
    Move lastMove;
    std::shared_ptr<SearchNode> parent;

    SearchNode(Piece p, const Move& m, std::shared_ptr<SearchNode> par)
        : piece(std::move(p)), lastMove(m), parent(std::move(par)) {}
  };

  /**
   * @brief Hash function for PieceState
   */
  struct PieceStateHash {
    size_t operator()(const PieceState& state) const {
      return PieceState::Hash{}(state);
    }
  };

  /**
   * @brief Equality function for PieceState
   */
  struct PieceStateEqual {
    bool operator()(const PieceState& lhs, const PieceState& rhs) const {
      return lhs == rhs;
    }
  };

  /**
   * @brief Reconstruct the path from the search result
   *
   * @param node The final search node
   * @return Vector of moves to reach the target
   */
  [[nodiscard]] static std::vector<Move>
  reconstructPath(std::shared_ptr<SearchNode>& node);

  /**
   * @brief Check if a move is valid in the current game state
   *
   * @param gameState The current game state
   * @param piece The piece to move
   * @param move The move to check
   * @return true if the move is valid, false otherwise
   */
  [[nodiscard]] bool isValidMove(const GameState& gameState, const Piece& piece,
                                 const Move& move) const;

  /**
   * @brief Apply a move to a piece (helper method for search algorithm)
   *
   * Note: This is different from GameState::applyMove as it doesn't modify the
   * game state, but instead returns a new piece with the move applied.
   *
   * @param gameState The current game state
   * @param piece The piece to move
   * @param move The move to apply
   * @return The resulting piece after the move
   */
  [[nodiscard]] Piece applyMove(const GameState& gameState, const Piece& piece,
                                const Move& move) const;

  /**
   * @brief Check if a piece is at a landing position
   *
   * @param gameState The current game state
   * @param piece The piece to check
   * @return true if the piece is at a landing position, false otherwise
   */
  [[nodiscard]] bool isAtLandingPosition(const GameState& gameState,
                                         const Piece& piece) const;
};

} // namespace tetris