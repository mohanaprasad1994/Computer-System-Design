section  .text
    global _start

_start:

    ;open the file for reading
    mov eax, 5             ;opening a file
    mov ebx, in_file
    mov ecx, 0             ;for read only access
    int  0x80
            
    mov  [fd_in], eax      ;store file descriptor value
    
    ;create the file
    mov  eax, 8
    mov  ebx, out_file
    mov  ecx, 0xffff       ;read, write and execute by all
    int  0x80              ;call kernel
    
    ;open file for writting
    mov eax, 5          
    mov ebx, out_file
    mov ecx, 2             ;for read only access
    int  0x80
    
    mov [fd_out], eax
        
    
    ;read from file
    L1: 
    mov eax, 3
    mov ebx, 3
    mov ecx, info
    mov edx, 100
    int 0x80
    
    ;write to file
    mov  edx,eax         ;number of bytes
    mov  ecx, info       ;message to write
    mov  ebx, 5          ;file descriptor 
    mov  eax,4           ;system call number (sys_write)
    int  0x80            ;call kernel
            
    
    CMP eax,0            ;eax carries the len read
    JNE L1
    
    ; close the in_file
    mov eax, 6
    mov ebx, [fd_in]
    
    ; close the out_file
    mov eax, 6
    mov ebx, [fd_out]
     
    ;exit program
    mov eax, 1
    mov ebx, 0
    int 0x80
    
section .data
    in_file db 'input.txt', 0  ; 0 at the end to show that
    out_file db 'output.txt', 0  ; 0 at the end to show that
    
section .bss
    fd_out resb 1
    fd_in  resb 1
    info resb  100
