# Pseudo Malloc

Progetto per l'esame di **Sistemi Operativi**.

Implementazione di un *replacement* per `malloc` che gestisce le richieste in due modi:
- richieste **piccole** (< 1/4 di pagina): tentativo con **buddy allocator** (1 MB, gestito con bitmap); se fallisce, si usa `mmap` come fallback
- richieste **grandi** (≥ 1/4 di pagina): `mmap` diretta

Il cuore del progetto è il **buddy allocator**, utilizzabile sia tramite il wrapper `pseudo_malloc` sia direttamente (ad es. nella shell interattiva).

## Struttura del progetto

| File | Descrizione |
|---|---|
| `pseudo_malloc.c/h` | Wrapper che sceglie buddy allocator o mmap in base alla dimensione |
| `buddy_allocator.c/h` | Buddy allocator con splitting/merging (usa `mmap` per il pool di memoria) |
| `bitmap_tree.c/h` | Albero binario implicito con bitmap (*used* e *split*) per tracciare i blocchi |
| `mmap_allocator.c/h` | Allocazione diretta con `mmap`/`munmap` per richieste grandi |
| `shell.c/h` | Shell interattiva che opera direttamente sul buddy allocator |
| `tests.c/h` | Suite di test automatici |
| `intmath.c/h` | Utility matematiche (potenze di 2, logaritmi) |
| `defaults.h` | Contiene i parametri predefiniti |
| `main.c` | Entry point |

## Buddy allocator

- Gestisce un'area di memoria (default 1 MB) divisa in blocchi di dimensione potenza di 2.
- La memoria fisica è ottenuta con `mmap` anonima.
- Alloca suddividendo ricorsivamente un blocco libero fino al minimo blocco sufficiente (`min_block`, default 32 byte).
- Al `free`, se il *buddy* (fratello) è anch'esso libero, i due blocchi vengono fusi ricorsivamente.
- Lo stato è tracciato con due bitmap per livello dell'albero: una per blocchi **usati/liberi**, una per blocchi **splittati/non splittati**.
- Esporta funzioni di debugging: `buddy_dump_tree()` e `buddy_print_stats()`.

## Mmap allocator

- Semplice wrapper per le syscall `mmap`/`munmap`.
- Usa `mmap` con flag `MAP_PRIVATE | MAP_ANONYMOUS` per allocare memoria fisica.
- Viene impiegato da `pseudo_malloc` per richieste grandi (≥ 1/4 di pagina) e come fallback quando il buddy allocatore è saturo.
- Funzioni: `mmap_alloc(size)` e `mmap_free(ptr, size)`.

## Compilazione

```bash
make          # compila con -O3
make debug    # compila con -Wall -Wextra -g
make clean    # rimuove file oggetto ed eseguibili
```

## Utilizzo

```bash
./pseudo_malloc test                      # esegue i test automatici
./pseudo_malloc shell                     # shell sul buddy allocator (1 MB, min_block 32)
./pseudo_malloc shell 65536 16            # buddy 64 KB, min_block 16
./pseudo_malloc help                      # mostra i comandi disponibili
```

### Comandi della shell

| Comando | Descrizione |
|---|---|
| `alloc <size>` | Alloca `<size>` byte, mostra il puntatore |
| `free <pointer>` | Libera la memoria al puntatore (esadecimale) |
| `dump` | Mostra l'albero dei blocchi del buddy allocator |
| `stats` | Mostra statistiche (allocazioni, split, merge, ...) |
| `help` | Elenco comandi |
| `exit` | Esci |

## Statistiche tracciate (buddy allocator)

- `allocations` / `deallocations` — numero di allocazioni e deallocazioni
- `failed_allocations` — allocazioni fallite
- `splits` / `merges` — numero di split e merge di blocchi
- `used_memory` — memoria attualmente in uso

## Test

Il comando `test` esegue test sia sul wrapper `pseudo_malloc`/`pseudo_free` sia direttamente sul buddy allocator.
