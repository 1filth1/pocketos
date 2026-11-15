#pragma once

// Five in a Row (Gomoku), human vs. computer. The 15x15 board and current
// position are persisted in Preferences; no SD card or board configuration
// changes are required.
static constexpr uint8_t FR_SIZE = 15;
static constexpr int32_t FR_WIN_SCORE = 10000000L;

struct FiveRowState {
  uint32_t magic;
  uint8_t cells[FR_SIZE][FR_SIZE]; // 0 empty, 1 human, 2 computer
  uint8_t cursorX, cursorY;
  uint16_t moves;
  uint8_t winner; // 0 none, 1 human, 2 computer, 3 draw
  uint8_t finished;
} fiveRow;

bool frInside(int x, int y) {
  return x >= 0 && x < FR_SIZE && y >= 0 && y < FR_SIZE;
}

void frSave() {
  prefs.putBytes("fiveRow", &fiveRow, sizeof(fiveRow));
}

void frNewGame() {
  memset(&fiveRow, 0, sizeof(fiveRow));
  fiveRow.magic = 0x35494E41; // "5INA"
  fiveRow.cursorX = FR_SIZE / 2;
  fiveRow.cursorY = FR_SIZE / 2;
  frSave();
}

bool frHasFive(int x, int y, uint8_t stone) {
  static const int8_t dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
  for (const auto &d : dirs) {
    int count = 1;
    for (int sign = -1; sign <= 1; sign += 2) {
      int xx = x + d[0] * sign, yy = y + d[1] * sign;
      while (frInside(xx, yy) && fiveRow.cells[yy][xx] == stone) {
        count++;
        xx += d[0] * sign;
        yy += d[1] * sign;
      }
    }
    if (count >= 5) return true;
  }
  return false;
}

// Scores a hypothetical stone by the contiguous run and its open ends.
int32_t frLineScore(int x, int y, uint8_t stone, int dx, int dy) {
  int count = 1;
  int open = 0;
  for (int sign = -1; sign <= 1; sign += 2) {
    int xx = x + dx * sign, yy = y + dy * sign;
    while (frInside(xx, yy) && fiveRow.cells[yy][xx] == stone) {
      count++;
      xx += dx * sign;
      yy += dy * sign;
    }
    if (frInside(xx, yy) && fiveRow.cells[yy][xx] == 0) open++;
  }
  if (count >= 5) return FR_WIN_SCORE;
  if (count == 4) return open == 2 ? 1000000L : (open == 1 ? 120000L : 0);
  if (count == 3) return open == 2 ? 18000L : (open == 1 ? 1800L : 0);
  if (count == 2) return open == 2 ? 350L : (open == 1 ? 60L : 0);
  return open ? 8 : 0;
}

int32_t frMoveScore(int x, int y, uint8_t stone) {
  static const int8_t dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
  int32_t total = 0;
  for (const auto &d : dirs)
    total += frLineScore(x, y, stone, d[0], d[1]);
  return total;
}

void frComputerMove() {
  int bestX = -1, bestY = -1;
  int winX = -1, winY = -1;
  int blockX = -1, blockY = -1;
  int32_t bestScore = -1;
  int32_t bestBlockScore = -1;
  bool hasStone = false;

  for (int y = 0; y < FR_SIZE; y++)
    for (int x = 0; x < FR_SIZE; x++)
      if (fiveRow.cells[y][x]) hasStone = true;

  // Prefer the centre for the opening move.
  if (!hasStone) {
    bestX = bestY = FR_SIZE / 2;
  } else {
    for (int y = 0; y < FR_SIZE; y++) {
      for (int x = 0; x < FR_SIZE; x++) {
        if (fiveRow.cells[y][x]) continue;
        bool nearby = false;
        for (int yy = max(0, y - 2); yy <= min(int(FR_SIZE) - 1, y + 2) && !nearby; yy++)
          for (int xx = max(0, x - 2); xx <= min(int(FR_SIZE) - 1, x + 2); xx++)
            if (fiveRow.cells[yy][xx]) { nearby = true; break; }
        if (!nearby) continue;

        int32_t attack = frMoveScore(x, y, 2);
        int32_t defence = frMoveScore(x, y, 1);

        // A winning move must always beat a defensive move. If no win is
        // available, block an immediate human win before considering patterns.
        if (attack >= FR_WIN_SCORE && winX < 0) {
          winX = x;
          winY = y;
        }
        if (defence >= FR_WIN_SCORE && defence > bestBlockScore) {
          bestBlockScore = defence;
          blockX = x;
          blockY = y;
        }

        // Mild centre preference makes equal-scoring moves less arbitrary.
        int centreBonus = max(0, 30 - abs(x - int(FR_SIZE / 2)) - abs(y - int(FR_SIZE / 2)));
        int32_t score = attack + defence + centreBonus;
        if (score > bestScore) {
          bestScore = score;
          bestX = x;
          bestY = y;
        }
      }
    }
    if (winX >= 0) {
      bestX = winX;
      bestY = winY;
    } else if (blockX >= 0) {
      bestX = blockX;
      bestY = blockY;
    }
  }

  if (bestX < 0) return;
  fiveRow.cells[bestY][bestX] = 2;
  fiveRow.moves++;
  if (frHasFive(bestX, bestY, 2)) {
    fiveRow.winner = 2;
    fiveRow.finished = 1;
  } else if (fiveRow.moves >= FR_SIZE * FR_SIZE) {
    fiveRow.winner = 3;
    fiveRow.finished = 1;
  }
}

void drawFiveRow() {
  const bool previousGameUpdates = epd.gameUpdates;
  epd.gameUpdates = true;
  beginDraw();
  do {
    String state = fiveRow.winner == 1 ? "YOU WIN" :
                   fiveRow.winner == 2 ? "COMPUTER WINS" :
                   fiveRow.winner == 3 ? "DRAW" :
                   (fiveRow.moves % 2 == 0 ? "YOUR TURN" : "COMPUTER TURN");
    header("5 IN A ROW", state, "ARROWS: MOVE  ENTER: PLACE  N: NEW GAME  ESC: GAMES");

    const int ox = 92, oy = 94, cell = 22;
    for (int i = 0; i < FR_SIZE; i++) {
      int p = ox + i * cell;
      int q = oy + i * cell;
      epd.drawLine(ox, q, ox + (FR_SIZE - 1) * cell, q, GxEPD_BLACK);
      epd.drawLine(p, oy, p, oy + (FR_SIZE - 1) * cell, GxEPD_BLACK);
    }
    for (int y = 0; y < FR_SIZE; y++) {
      for (int x = 0; x < FR_SIZE; x++) {
        int px = ox + x * cell, py = oy + y * cell;
        if (fiveRow.cells[y][x] == 1) {
          epd.fillCircle(px, py, 8, GxEPD_BLACK);
        } else if (fiveRow.cells[y][x] == 2) {
          epd.fillCircle(px, py, 8, GxEPD_WHITE);
          epd.drawCircle(px, py, 8, GxEPD_BLACK);
        }
      }
    }
    int cx = ox + fiveRow.cursorX * cell;
    int cy = oy + fiveRow.cursorY * cell;
    epd.drawRect(cx - 10, cy - 10, 20, 20, GxEPD_BLACK);
    epd.drawRect(cx - 8, cy - 8, 16, 16, GxEPD_BLACK);

    text("GOMOKU", 470, 115, true);
    text("YOU: BLACK", 470, 160);
    text("CPU: WHITE", 470, 193);
    text("MOVES: " + String(fiveRow.moves), 470, 245);
    text("BOARD: 15 x 15", 470, 282);
    if (fiveRow.finished) {
      text("N: PLAY AGAIN", 470, 350, true);
      text("ESC: RETURN", 470, 386);
    } else {
      text(fiveRow.moves % 2 == 0 ? "YOUR MOVE" : "CPU MOVE", 470, 350, true);
      text("N: RESTART", 470, 386);
    }
  } while (epd.nextPage());
  epd.gameUpdates = previousGameUpdates;
}

bool frBoardHasFive(uint8_t stone) {
  for (int y = 0; y < FR_SIZE; y++)
    for (int x = 0; x < FR_SIZE; x++)
      if (fiveRow.cells[y][x] == stone && frHasFive(x, y, stone)) return true;
  return false;
}

bool frStateValid() {
  if (fiveRow.magic != 0x35494E41 || fiveRow.cursorX >= FR_SIZE ||
      fiveRow.cursorY >= FR_SIZE || fiveRow.moves > FR_SIZE * FR_SIZE ||
      fiveRow.winner > 3 || fiveRow.finished > 1) return false;

  uint16_t human = 0, computer = 0;
  for (int y = 0; y < FR_SIZE; y++) {
    for (int x = 0; x < FR_SIZE; x++) {
      uint8_t cell = fiveRow.cells[y][x];
      if (cell > 2) return false;
      if (cell == 1) human++;
      else if (cell == 2) computer++;
    }
  }

  if (human + computer != fiveRow.moves || human < computer || human > computer + 1)
    return false;
  if ((fiveRow.winner == 0) != (fiveRow.finished == 0)) return false;

  bool humanWon = frBoardHasFive(1);
  bool computerWon = frBoardHasFive(2);
  if (fiveRow.winner == 0) return !humanWon && !computerWon && fiveRow.moves < FR_SIZE * FR_SIZE;
  if (fiveRow.winner == 1) return human == computer + 1 && humanWon && !computerWon;
  if (fiveRow.winner == 2) return human == computer && computerWon && !humanWon;
  return fiveRow.moves == FR_SIZE * FR_SIZE && !humanWon && !computerWon;
}

void startFiveRow() {
  bool valid = prefs.getBytesLength("fiveRow") == sizeof(fiveRow) &&
               prefs.getBytes("fiveRow", &fiveRow, sizeof(fiveRow)) == sizeof(fiveRow) &&
               frStateValid();
  if (!valid) frNewGame();
  view = FIVE_ROW;
}

void frPlaceStone() {
  if (fiveRow.finished || fiveRow.cells[fiveRow.cursorY][fiveRow.cursorX]) return;
  int x = fiveRow.cursorX, y = fiveRow.cursorY;
  fiveRow.cells[y][x] = 1;
  fiveRow.moves++;
  if (frHasFive(x, y, 1)) {
    fiveRow.winner = 1;
    fiveRow.finished = 1;
  } else if (fiveRow.moves >= FR_SIZE * FR_SIZE) {
    fiveRow.winner = 3;
    fiveRow.finished = 1;
  } else {
    frComputerMove();
  }
  frSave();
  drawFiveRow();
}

void frMoveCursor(int dx, int dy) {
  int x = constrain(int(fiveRow.cursorX) + dx, 0, int(FR_SIZE) - 1);
  int y = constrain(int(fiveRow.cursorY) + dy, 0, int(FR_SIZE) - 1);
  if (x == fiveRow.cursorX && y == fiveRow.cursorY) return;
  fiveRow.cursorX = x;
  fiveRow.cursorY = y;
  // Cursor motion is transient: persist only after a move, restart or exit.
  // This avoids a verified SD-card settings write for every arrow press.
  drawFiveRow();
}

void fiveRowKey(uint8_t k) {
  if (k == 0xB4) frMoveCursor(-1, 0);
  else if (k == 0xB7) frMoveCursor(1, 0);
  else if (k == 0xB5) frMoveCursor(0, -1);
  else if (k == 0xB6) frMoveCursor(0, 1);
  else if (k == 10 || k == 13) frPlaceStone();
  else if (k == 'n' || k == 'N' || k == 'r' || k == 'R') {
    frNewGame();
    drawFiveRow();
  }
}
