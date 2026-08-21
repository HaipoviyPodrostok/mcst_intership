; ModuleID = 'tests/input.c'
source_filename = "tests/input.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @foo(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = mul nsw i32 %3, 2
  ret i32 %4
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @bar(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %5 = load i32, ptr %3, align 4
  %6 = load i32, ptr %4, align 4
  %7 = call i32 @foo(i32 noundef %6)
  %8 = mul nsw i32 %7, 2
  %9 = add nsw i32 %5, %8
  ret i32 %9
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @fez(i32 noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %4, align 4
  store i32 %1, ptr %5, align 4
  store i32 %2, ptr %6, align 4
  %7 = load i32, ptr %4, align 4
  %8 = load i32, ptr %4, align 4
  %9 = load i32, ptr %5, align 4
  %10 = call i32 @bar(i32 noundef %8, i32 noundef %9)
  %11 = mul nsw i32 %10, 2
  %12 = add nsw i32 %7, %11
  %13 = load i32, ptr %6, align 4
  %14 = mul nsw i32 %13, 3
  %15 = add nsw i32 %12, %14
  ret i32 %15
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 0, ptr %3, align 4
  store i32 %0, ptr %4, align 4
  store ptr %1, ptr %5, align 8
  store i32 123, ptr %6, align 4
  store i32 0, ptr %7, align 4
  %8 = load i32, ptr %6, align 4
  %9 = call i32 @foo(i32 noundef %8)
  %10 = load i32, ptr %7, align 4
  %11 = add nsw i32 %10, %9
  store i32 %11, ptr %7, align 4
  %12 = load i32, ptr %6, align 4
  %13 = load i32, ptr %7, align 4
  %14 = call i32 @bar(i32 noundef %12, i32 noundef %13)
  %15 = load i32, ptr %7, align 4
  %16 = add nsw i32 %15, %14
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %6, align 4
  %18 = load i32, ptr %7, align 4
  %19 = call i32 @fez(i32 noundef %17, i32 noundef %18, i32 noundef 123)
  %20 = load i32, ptr %7, align 4
  %21 = add nsw i32 %20, %19
  store i32 %21, ptr %7, align 4
  %22 = load i32, ptr %7, align 4
  ret i32 %22
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @req1(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  br label %3

3:                                                ; preds = %22, %10, %1
  %4 = load i32, ptr %2, align 4
  %5 = add nsw i32 %4, 1
  store i32 %5, ptr %2, align 4
  %6 = load i32, ptr %2, align 4
  %7 = icmp sgt i32 %6, 0
  br i1 %7, label %8, label %9

8:                                                ; preds = %3
  br label %10

9:                                                ; preds = %3
  br label %13

10:                                               ; preds = %8
  %11 = load i32, ptr %2, align 4
  %12 = add nsw i32 %11, 2
  store i32 %12, ptr %2, align 4
  br label %3

13:                                               ; preds = %21, %9
  %14 = load i32, ptr %2, align 4
  %15 = sub nsw i32 %14, 5
  store i32 %15, ptr %2, align 4
  br label %16

16:                                               ; preds = %13
  %17 = load i32, ptr %2, align 4
  %18 = mul nsw i32 %17, 7
  store i32 %18, ptr %2, align 4
  %19 = load i32, ptr %2, align 4
  %20 = icmp sgt i32 %19, 0
  br i1 %20, label %21, label %22

21:                                               ; preds = %16
  br label %13

22:                                               ; preds = %16
  br label %3
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @constant_fold_basic() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 10, ptr %1, align 4
  store i32 20, ptr %2, align 4
  %4 = load i32, ptr %1, align 4
  %5 = load i32, ptr %2, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, ptr %3, align 4
  %7 = load i32, ptr %3, align 4
  ret i32 %7
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @constant_fold_chain() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  %5 = load i32, ptr %1, align 4
  %6 = mul nsw i32 %5, 3
  store i32 %6, ptr %2, align 4
  %7 = load i32, ptr %2, align 4
  %8 = add nsw i32 %7, 7
  store i32 %8, ptr %3, align 4
  %9 = load i32, ptr %3, align 4
  %10 = sub nsw i32 %9, 2
  store i32 %10, ptr %4, align 4
  %11 = load i32, ptr %4, align 4
  ret i32 %11
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_true() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 1, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  store i32 42, ptr %1, align 4
  br label %7

6:                                                ; preds = %0
  store i32 99, ptr %1, align 4
  br label %7

7:                                                ; preds = %6, %5
  %8 = load i32, ptr %1, align 4
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_false() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  store i32 42, ptr %1, align 4
  br label %7

6:                                                ; preds = %0
  store i32 99, ptr %1, align 4
  br label %7

7:                                                ; preds = %6, %5
  %8 = load i32, ptr %1, align 4
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_icmp() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 10, ptr %2, align 4
  store i32 20, ptr %3, align 4
  %4 = load i32, ptr %2, align 4
  %5 = load i32, ptr %3, align 4
  %6 = icmp sgt i32 %4, %5
  br i1 %6, label %7, label %8

7:                                                ; preds = %0
  store i32 1, ptr %1, align 4
  br label %9

8:                                                ; preds = %0
  store i32 0, ptr %1, align 4
  br label %9

9:                                                ; preds = %8, %7
  %10 = load i32, ptr %1, align 4
  ret i32 %10
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_both_const_same(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %1
  store i32 42, ptr %3, align 4
  br label %8

7:                                                ; preds = %1
  store i32 42, ptr %3, align 4
  br label %8

8:                                                ; preds = %7, %6
  %9 = load i32, ptr %3, align 4
  ret i32 %9
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_both_const_diff(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %1
  store i32 10, ptr %3, align 4
  br label %8

7:                                                ; preds = %1
  store i32 20, ptr %3, align 4
  br label %8

8:                                                ; preds = %7, %6
  %9 = load i32, ptr %3, align 4
  ret i32 %9
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_one_dead_branch() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 1, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %0
  store i32 77, ptr %1, align 4
  br label %7

6:                                                ; preds = %0
  store i32 99, ptr %1, align 4
  br label %7

7:                                                ; preds = %6, %5
  %8 = load i32, ptr %1, align 4
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @nested_branches() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 5, ptr %1, align 4
  store i32 10, ptr %2, align 4
  %4 = load i32, ptr %1, align 4
  %5 = load i32, ptr %2, align 4
  %6 = icmp slt i32 %4, %5
  br i1 %6, label %7, label %15

7:                                                ; preds = %0
  %8 = load i32, ptr %1, align 4
  %9 = load i32, ptr %2, align 4
  %10 = add nsw i32 %8, %9
  %11 = icmp sgt i32 %10, 12
  br i1 %11, label %12, label %13

12:                                               ; preds = %7
  store i32 1, ptr %3, align 4
  br label %14

13:                                               ; preds = %7
  store i32 2, ptr %3, align 4
  br label %14

14:                                               ; preds = %13, %12
  br label %16

15:                                               ; preds = %0
  store i32 3, ptr %3, align 4
  br label %16

16:                                               ; preds = %15, %14
  %17 = load i32, ptr %3, align 4
  ret i32 %17
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @overdef_from_arg(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 10, ptr %3, align 4
  %5 = load i32, ptr %3, align 4
  %6 = load i32, ptr %2, align 4
  %7 = add nsw i32 %5, %6
  store i32 %7, ptr %4, align 4
  %8 = load i32, ptr %4, align 4
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @mul_by_zero() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 999, ptr %1, align 4
  store i32 0, ptr %2, align 4
  %3 = load i32, ptr %1, align 4
  %4 = load i32, ptr %2, align 4
  %5 = mul nsw i32 %3, %4
  ret i32 %5
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @complex_chain() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 2, ptr %1, align 4
  store i32 3, ptr %2, align 4
  %7 = load i32, ptr %1, align 4
  %8 = load i32, ptr %2, align 4
  %9 = add nsw i32 %7, %8
  store i32 %9, ptr %3, align 4
  %10 = load i32, ptr %3, align 4
  %11 = mul nsw i32 %10, 2
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %4, align 4
  %13 = sub nsw i32 %12, 1
  store i32 %13, ptr %5, align 4
  %14 = load i32, ptr %5, align 4
  %15 = load i32, ptr %1, align 4
  %16 = add nsw i32 %14, %15
  store i32 %16, ptr %6, align 4
  %17 = load i32, ptr %6, align 4
  ret i32 %17
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @loop_with_const_exit(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  store i32 0, ptr %4, align 4
  br label %5

5:                                                ; preds = %13, %1
  %6 = load i32, ptr %4, align 4
  %7 = load i32, ptr %2, align 4
  %8 = icmp slt i32 %6, %7
  br i1 %8, label %9, label %16

9:                                                ; preds = %5
  %10 = load i32, ptr %4, align 4
  %11 = load i32, ptr %3, align 4
  %12 = add nsw i32 %11, %10
  store i32 %12, ptr %3, align 4
  br label %13

13:                                               ; preds = %9
  %14 = load i32, ptr %4, align 4
  %15 = add nsw i32 %14, 1
  store i32 %15, ptr %4, align 4
  br label %5, !llvm.loop !6

16:                                               ; preds = %5
  %17 = load i32, ptr %3, align 4
  ret i32 %17
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @multi_phi_cascade(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %7 = load i32, ptr %3, align 4
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %10

9:                                                ; preds = %2
  store i32 10, ptr %5, align 4
  br label %11

10:                                               ; preds = %2
  store i32 20, ptr %5, align 4
  br label %11

11:                                               ; preds = %10, %9
  %12 = load i32, ptr %4, align 4
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %17

14:                                               ; preds = %11
  %15 = load i32, ptr %5, align 4
  %16 = add nsw i32 %15, 1
  store i32 %16, ptr %6, align 4
  br label %20

17:                                               ; preds = %11
  %18 = load i32, ptr %5, align 4
  %19 = add nsw i32 %18, 2
  store i32 %19, ptr %6, align 4
  br label %20

20:                                               ; preds = %17, %14
  %21 = load i32, ptr %6, align 4
  ret i32 %21
}

attributes #0 = { noinline nounwind sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 22.1.8"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
