                .data
A:              .word   1, 3, 1, 6, 4
                .word   2, 4, 3, 9, 5
mult:   .word   0

        .code
        daddi   $1, $0, A      ; *A[0]
         
        daddi   $6, $0, 10     ; $6 = N ;; N = 10
        lw      $9, 0($1)      ; $9 = A[0]  ;; mult
        daddi   $1, $1, 8      ; Move to next index in A
        
        lw      $12, 0($1)     ; $12 = A[i]
        daddi   $1, $1, 8      ; Move to next index in A
        dmul    $12, $12, $9   ; $12 = A[i]*mult
        dadd    $9, $9, $12    ; mult = mult + A[i]*mult
        daddi   $5, $0, 2      ; $5 = 2 ;; Increase Loop Counter   

loop:   lw      $12, 0($1)     ; $12 = A[i]
        daddi   $1, $1, 8      ; Move to next index in A
        dmul    $12, $12, $9   ; $12 = A[i]*mult
        dadd    $9, $9, $12    ; mult +=A[i]*mult
        
        lw      $13, 0($1)     ; $13 = A[i+1]
        daddi   $1, $1, 8      ; Move to next index in A
        dmul    $13, $13, $9   ; $13 = A[i+1]*mult
        dadd    $9, $9, $13    ; mult +=A[i+1]*mult
        
        lw      $14, 0($1)     ; $14 = A[i+2]
        daddi   $1, $1, 8      ; Move to next index in A
        dmul    $14, $14, $9   ; $14 = A[i+2]*mult
        dadd    $9, $9, $14    ; mult +=A[i+2]*mult
        
        lw      $15, 0($1)     ; $15 = A[i+3]
        daddi   $1, $1, 8      ; Move to next index in A
        dmul    $15, $15, $9   ; $15 = A[i+3]*mult
        daddi   $5, $5, 4      ; i += 4
        dadd    $9, $9, $15    ; mult +=A[i+3]*mult
        
        bne     $6, $5, loop   ; Exit loop if i == N
        
        sw      $9, mult($0)   ; Store result
        halt

;; Expected result: mult = f6180 (hex), 1008000 (dec)