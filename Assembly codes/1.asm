section .text
  global _start  

_start:   ; tell linker entry point
  
  ; take input for m
  mov  edx, len_input_m  ; message length
  mov  ecx, input_m      ; message to write
  mov  ebx, 1            ; file descriptor (stdout)
  mov  eax, 4            ; system call number for sys_write
  int  0x80              ; interrupt for system call

  mov  edx, 4            ; message length                   
  mov  ecx, var_m        ; location to read to
  mov  ebx, 2            ; file descriptor (stdin)            
  mov  eax, 3            ; system call number for sys_read
  int  0x80        

  ; take input for n
  mov  edx, len_input_n
  mov  ecx, input_n   
  mov  ebx, 1    
  mov  eax, 4    
  int  0x80  

  mov  edx, 10   
  mov  ecx, var_n   
  mov  ebx, 2    
  mov  eax, 3    
  int  0x80  
  
  ; convert m and n from string to int
  mov  edx, var_m
  mov  eax, 0
  call convert_string_to_int
  mov  [var_m], eax
  mov  edx, var_n
  mov  eax, 0
  call convert_string_to_int
  mov [var_n], eax

  ; compute ackermann function for the given m and n
  mov ebx, [var_m]
  mov ecx, [var_n]

  ; check if m and n are within a certain range so that
  ; function fits a 32 bit value
  cmp ebx, 4
  ja error
  cmp ebx, 4
  jne check_n
  cmp ecx, 1
  ja error
check_n:
  cmp ebx, 3
  jne input_ok
  cmp ecx, 28
  je error

input_ok:  
  call  ackermann
  mov  [ackermann_output],  eax
  
  ; output the computed value 
  mov  edx, len_output   
  mov  ecx, output   
  mov  ebx, 1    
  mov  eax, 4    
  int  0x80  

  mov eax, [ackermann_output]
  call print_num
  call print_newline
  jmp exit

error:
  mov  edx, len_error   
  mov  ecx, error_msg
  mov  ebx, 1    
  mov  eax, 4    
  int  0x80  

exit:
  mov  eax, 1    ;system call number (sys_exit)
  int  0x80    

; function which converts string to integer
; edx - pointer to string
; eax - return value, should be set to 0
; scans string byte by byte and computes the integer
convert_string_to_int:
  movzx ecx, byte [edx]
  cmp ecx, 48
  jb done
  cmp ecx, 57
  ja done
  sub ecx, 48
  imul eax, 10
  add eax, ecx
  inc edx
  call convert_string_to_int
done:
  ret

; function which computes the Ackermann function
; ebx - m
; ecx - n
; eax - return value
ackermann:
  cmp ebx, 0
  jne condition2
  inc ecx
  mov eax, ecx
  ret
condition2:
  cmp ecx, 0
  jne condition3
  dec ebx
  inc ecx
  call ackermann
  ret
condition3:
  push ebx
  dec ecx
  call ackermann
  mov ecx, eax
  pop ebx
  dec ebx
  call ackermann
  ret

; function which prints an integer as a string
; eax - should contain the number
; recursively compute and store reminders on dividing by 10
; push these to stack and print them in reverse order
print_num:
  mov edx, 0
  div dword [const_ten]
  cmp eax, 0
  je print_reminder
  push edx
  call print_num
  pop edx
print_reminder:
  add edx, 48
  mov [reminder], edx
  call print_digit
  ret

; function which prints a single digit to the screen
; digit should be stored in memory address reminder
print_digit:
  mov  edx, 4  
  mov  ecx, reminder  
  mov  ebx, 1    
  mov  eax, 4    
  int  0x80  
  ret

; function which outputs a newline character
print_newline:
  mov  edx, 1 
  mov  ecx, newline_char  
  mov  ebx, 1
  mov  eax, 4
  int  0x80  
  ret

section .data
; interface message for input m
input_m dd 'Enter m:', 0xa
len_input_m equ $ - input_m

; interface message for input n
input_n dd 'Enter n:', 0xa
len_input_n equ $ - input_n

; interface message for output
output dd 'Answer is:', 0xa
len_output equ $ - output

; interface message for output
error_msg dd 'Too large input', 0xa
len_error equ $ - error_msg

; constant with value 10
const_ten dd 10

; constant with ascii value for newline
newline_char db 0xa

section .bss
; variable which contains ackermann value
ackermann_output resb 4

; variables holding m and n
var_m resb 4
var_n resb 10
reminder resb 4
