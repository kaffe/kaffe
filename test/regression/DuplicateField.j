
.class DuplicateField
.super java/lang/Object

.field public foo I

.field public foo I

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: DuplicateField, field: foo) Duplicate field.
