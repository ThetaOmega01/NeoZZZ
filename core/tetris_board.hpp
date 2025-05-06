#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace tetris {

/**
 * @brief Maximum height of a Tetris board
 */
constexpr int maxHeight{40};

/**
 * @brief Maximum width of a Tetris board
 */
constexpr int maxWidth{32};

/**
 * @class Board
 * @brief Tetris game board
 *
 * (0,0) is the bottom-left corner.
 * The board supports a maximum size of maxHeight * maxWidth.
 */
class Board {
public:
  /**
   * @brief Default constructor
   */
  Board() = default;

  /**
   * @brief Construct a board with specified dimensions
   *
   * @param width The width of the board
   * @param height The height of the board
   */
  Board(int width, int height);

  /**
   * @brief Copy constructor
   */
  Board(const Board &other) = default;

  /**
   * @brief Move constructor
   */
  Board(Board &&other) noexcept = default;

  /**
   * @brief Copy assignment operator
   */
  Board &operator=(const Board &other) = default;

  /**
   * @brief Move assignment operator
   */
  Board &operator=(Board &&other) noexcept = default;

  /**
   * @brief Equality operator
   */
  bool operator==(const Board &other) const;

  /**
   * @brief Inequality operator
   */
  bool operator!=(const Board &other) const;

  /**
   * @brief Check if a cell at the given coordinates is filled
   *
   * @param x X-coordinate (column)
   * @param y Y-coordinate (row)
   * @return true if the cell is filled, false otherwise
   */
  [[nodiscard]] bool isFilled(int x, int y) const;

  /**
   * @brief Fill a cell at the given coordinates
   *
   * @param x X-coordinate (column)
   * @param y Y-coordinate (row)
   */
  void fillCell(int x, int y);

  /**
   * @brief Clear a cell at the given coordinates
   *
   * @param x X-coordinate (column)
   * @param y Y-coordinate (row)
   */
  void clearCell(int x, int y);

  /**
   * @brief Get the width of the board
   *
   * @return The width
   */
  [[nodiscard]] int getWidth() const { return m_width; }

  /**
   * @brief Get the height of the board
   *
   * @return The height
   */
  [[nodiscard]] int getHeight() const { return m_height; }

  /**
   * @brief Get the current highest filled cell in the board
   *
   * @return The height of the highest filled cell
   */
  [[nodiscard]] int getRoof() const { return m_roof; }

  /**
   * @brief Get the number of filled cells in the board
   *
   * @return The count of filled cells
   */
  [[nodiscard]] int getFilledCellCount() const { return m_filledCellCount; }

  /**
   * @brief Get the height of the highest filled cell in a column
   *
   * @param column The column index
   * @return The height of the highest filled cell in the column
   */
  [[nodiscard]] int getColumnHeight(int column) const;

  /**
   * @brief Clear filled rows and update the board state
   *
   * @return The number of rows cleared
   */
  int clearFilledRows();

  /**
   * @brief Check if a row is completely filled
   *
   * @param row The row index
   * @return true if the row is filled, false otherwise
   */
  [[nodiscard]] bool isRowFilled(int row) const;

  /**
   * @brief Get a read-only view of the row data
   *
   * @return A span of the row data
   */
  [[nodiscard]] std::span<const std::uint32_t> getRowData() const;

  /**
   * @brief Get a read-only view of the column heights
   *
   * @return A span of the column heights
   */
  [[nodiscard]] std::span<const int> getColumnHeights() const;

private:
  /**
   * @brief Update all column heights and roof after board changes
   */
  void updateHeights();

  /**
   * @brief Update a specific column height and recalculate roof
   *
   * @param column The column to update
   */
  void updateHeights(int column);

private:
  std::array<std::uint32_t, maxHeight>
      m_rows{}; ///< Bit representation of each row
  std::array<int, maxWidth> m_columnHeights{}; ///< Height of each column
  int m_width{0};                              ///< Width of the board
  int m_height{0};                             ///< Height of the board
  int m_roof{0};                               ///< Current highest filled cell
  int m_filledCellCount{0};                    ///< Number of filled cells
  std::uint32_t m_fullRowMask{0};              ///< Bit mask for a full row
};

} // namespace tetris
