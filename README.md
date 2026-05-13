To give your README.md a more professional and modern feel, I've added structured formatting, clear visual hierarchy, and iconography. This version uses clean Markdown blocks to make the technical details—like the bitboard architecture and Minimax algorithm—pop for anyone viewing your portfolio.

---

# ♟️ Custom C++ Chess Engine

A lightweight, high-performance chess engine built with **C++20** and **SFML**. This project features a custom **bitboard-based** move generator and an AI opponent powered by a **Minimax algorithm** with **Alpha-Beta pruning**.

---

## 🚀 Features

- **Bitboard Architecture**
  Uses 64-bit integers to represent the board for lightning-fast move generation and legality checking.
- **AI Opponent**
  An optimized Minimax algorithm enhanced with Alpha-Beta pruning, capable of searching multiple plies deep while maintaining smooth frame rates.
- **Chess Logic**
  Full implementation of standard rules including:
- Castling (Kingside and Queenside).
- En Passant captures.
- Pawn Promotion with an interactive UI selection.
- Checkmate and Stalemate detection.

- **Interactive UI**
  Clean graphical interface built with SFML, featuring legal move highlighting and turn indicators.

---

## 🧠 Engine Details

The AI evaluates positions using **Piece-Square Tables (PST)**, allowing it to understand not just the value of pieces, but their optimal placement on the board (e.g., controlling the center, developing knights).

> [!TIP]
> At a depth of 5, the engine provides a challenging experience for intermediate casual players.

---

## 🛠️ Build Instructions

### Prerequisites

- **C++20** compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+).
- **SFML 3.0** installed and linked.

### Compiling

If you are using MinGW/GCC, you can build the project from the terminal:

```bash
g++ -std=c++20 src/*.cpp -o chess_engine -lsfml-graphics -lsfml-window -lsfml-system

```

---

## 🎮 How to Play

1. **Select a Piece:** Left-click a piece to see its legal moves (highlighted in blue).
2. **Move:** Click a highlighted square to complete the move.
3. **Promote:** When a pawn reaches the final rank, a selection panel will appear. Click the desired piece to promote.
4. **AI Turn:** Once you move, the AI will automatically begin calculating its response.

---

## 🎨 Credits

### Art Assets

The beautiful pixel-art chess pieces used in this project were created by **Dani Maccari**.

- 👉 [Pixel Chess by Dani Maccari](https://www.google.com/search?q=https://danimaccari.itch.io/pixel-chess)

---

## 📜 License

This project is for educational purposes. Please ensure you respect the license of the art assets provided by Dani Maccari when redistributing.
