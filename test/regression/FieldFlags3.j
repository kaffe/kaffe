
.class interface abstract FieldFlags3
.super java/lang/Object

.field foo I

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: FieldFlags3, field: foo) Interface fields must have the public, static, and final flags set
