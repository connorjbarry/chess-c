#include "../include/defs.h"

const int pv_size = 0x100000 * 2;

int get_pvline(S_BOARD *pos, const int depth) {
  ASSERT(depth < MAX_DEPTH && depth >= 1);

  int move = probe_pvtable(pos);
  int count = 0;

  while (move != NOMOVE && count < depth) {
    ASSERT(count < MAX_DEPTH);

    if (move_exists(pos, move)) {
      make_move(pos, move);
      pos->pv_arr[count++] = move;
    } else {
      break;
    }
    move = probe_pvtable(pos);
  }

  while (pos->ply > 0) {
    undo_move(pos);
  }

  return count;
}

void clear_pvtable(S_PVTABLE *pvt) {
  S_PVENTRY *pve;

  for (pve = pvt->pTable; pve < pvt->pTable + pvt->numEntries; pve++) {
    pve->posKey = 0ULL;
    pve->move = NOMOVE;
  }
}

void init_pvtable(S_PVTABLE *pvt) {
  pvt->numEntries = pv_size / sizeof(S_PVENTRY);
  pvt->numEntries -= 2;

  if (pvt->pTable != NULL) {
    free(pvt->pTable);
  }
  pvt->pTable = (S_PVENTRY *)malloc(pvt->numEntries * sizeof(S_PVENTRY));
  clear_pvtable(pvt);

  printf("PvTable init complete, %d entries\n", pvt->numEntries);
}

void store_pvmove(const S_BOARD *pos, int move) {
  int idx = pos->posKey % pos->pvTable->numEntries;
  ASSERT(idx >= 0 && idx <= pos->pvTable->numEntries - 1);

  pos->pvTable->pTable[idx].move = move;
  pos->pvTable->pTable[idx].posKey = pos->posKey;
}

int probe_pvtable(const S_BOARD *pos) {
  int idx = pos->posKey % pos->pvTable->numEntries;
  ASSERT(idx >= 0 && idx <= pos->pvTable->numEntries - 1);

  if (pos->pvTable->pTable[idx].posKey == pos->posKey) {
    return pos->pvTable->pTable[idx].move;
  }

  return NOMOVE;
}
