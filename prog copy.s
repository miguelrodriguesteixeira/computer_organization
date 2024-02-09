.data
StartAddress:   .word 0x10010000   # Example start address
EndLimite:      .word 0x10010010   # Example end address

.text
.global main

main:
    lw   $t0, StartAddress    # Load StartAddress into $t0
    lw   $t1, EndLimite       # Load EndLimite into $t1
    add  $t6, $zero, $zero    # Initialize $t6 to 0

Loop:
    lw   $t4, 0($t1)          # Load word at address $t1 into $t4 (assuming 32-bit words)
    sw   $t4, 4($t1)          # Store the word at address $t1 + 4
    add  $t6, $t4, $t6        # Add $t4 to $t6
    addi $t1, $t1, -4         # Decrement $t1 by 4 (assuming 32-bit words)
    bnez $t1, Loop            # Branch to Loop if $t1 is not zero

    sub  $t5, $t5, $t6        # Subtract $t6 from $t5

    # Exit the program (example: terminate program)
    li   $v0, 10              # Load exit system call code
    syscall                   # Make a system call to exit the program
