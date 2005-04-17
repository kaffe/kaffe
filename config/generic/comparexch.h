
/*
 * Do an atomic compare and exchange.  The address 'A' is checked against  
 * value 'O' and if they match it's exchanged with value 'N'.
 * We return '1' if the exchange is sucessful (that is if (*A) == (O)),
 * otherwise 0.
 */
#define COMPARE_AND_EXCHANGE(A,O,N) (atomic_compare_and_exchange_val_acq((A), (N), (O)) == (O))
