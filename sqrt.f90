program area
  implicit none
  
  real :: A, B
  double precision :: C
  
  A = 2.0
  
  B = sqrt(A)
  C = sqrt(A)
  
  print *, "SQRT (64-bit)(", A, ") = ", B
  print *, "SQRT (80-bit)(", A, ") = ", C

  stop
end program area
