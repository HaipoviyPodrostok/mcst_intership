; ModuleID = 'tests/input.ll'
source_filename = "tests/input.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @foo(i32 noundef %0) #0 {
  %2 = mul nsw i32 %0, 2
  ret i32 %2
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @bar(i32 noundef %0, i32 noundef %1) #0 {
  %3 = call i32 @foo(i32 noundef %1)
  %4 = mul nsw i32 %3, 2
  %5 = add nsw i32 %0, %4
  ret i32 %5
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @fez(i32 noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = call i32 @bar(i32 noundef %0, i32 noundef %1)
  %5 = mul nsw i32 %4, 2
  %6 = add nsw i32 %0, %5
  %7 = mul nsw i32 %2, 3
  %8 = add nsw i32 %6, %7
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = call i32 @foo(i32 noundef 123)
  %4 = add nsw i32 0, %3
  %5 = call i32 @bar(i32 noundef 123, i32 noundef %4)
  %6 = add nsw i32 %4, %5
  %7 = call i32 @fez(i32 noundef 123, i32 noundef %6, i32 noundef 123)
  %8 = add nsw i32 %6, %7
  ret i32 %8
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @req1(i32 noundef %0) #0 {
  br label %2

2:                                                ; preds = %15, %7, %1
  %.0 = phi i32 [ %0, %1 ], [ %8, %7 ], [ %12, %15 ]
  %3 = add nsw i32 %.0, 1
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %2
  br label %7

6:                                                ; preds = %2
  br label %9

7:                                                ; preds = %5
  %8 = add nsw i32 %3, 2
  br label %2

9:                                                ; preds = %14, %6
  %.1 = phi i32 [ %3, %6 ], [ %12, %14 ]
  %10 = sub nsw i32 %.1, 5
  br label %11

11:                                               ; preds = %9
  %12 = mul nsw i32 %10, 7
  %13 = icmp sgt i32 %12, 0
  br i1 %13, label %14, label %15

14:                                               ; preds = %11
  br label %9

15:                                               ; preds = %11
  br label %2
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @constant_fold_basic() #0 {
  %1 = add nsw i32 10, 20
  ret i32 30
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @constant_fold_chain() #0 {
  %1 = mul nsw i32 5, 3
  %2 = add nsw i32 15, 7
  %3 = sub nsw i32 22, 2
  ret i32 20
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_true() #0 {
  %1 = icmp ne i32 1, 0
  br label %2

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %2
  ret i32 42
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_false() #0 {
  %1 = icmp ne i32 0, 0
  br label %2

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %2
  ret i32 99
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @dead_branch_icmp() #0 {
  %1 = icmp sgt i32 10, 20
  br label %2

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %2
  ret i32 0
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_both_const_same(i32 noundef %0) #0 {
  %2 = icmp ne i32 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  br label %5

4:                                                ; preds = %1
  br label %5

5:                                                ; preds = %4, %3
  ret i32 42
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_both_const_diff(i32 noundef %0) #0 {
  %2 = icmp ne i32 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  br label %5

4:                                                ; preds = %1
  br label %5

5:                                                ; preds = %4, %3
  %.0 = phi i32 [ 10, %3 ], [ 20, %4 ]
  ret i32 20
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @phi_one_dead_branch() #0 {
  %1 = icmp ne i32 1, 0
  br label %2

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %2
  ret i32 77
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @nested_branches() #0 {
  %1 = icmp slt i32 5, 10
  br label %2

2:                                                ; preds = %0
  %3 = add nsw i32 5, 10
  %4 = icmp sgt i32 15, 12
  br label %5

5:                                                ; preds = %2
  br label %6

6:                                                ; preds = %5
  br label %7

7:                                                ; preds = %6
  ret i32 1
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @overdef_from_arg(i32 noundef %0) #0 {
  %2 = add nsw i32 10, %0
  ret i32 %2
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @mul_by_zero() #0 {
  %1 = mul nsw i32 999, 0
  ret i32 0
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @complex_chain() #0 {
  %1 = add nsw i32 2, 3
  %2 = mul nsw i32 5, 2
  %3 = sub nsw i32 10, 1
  %4 = add nsw i32 9, 2
  ret i32 11
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @loop_with_const_exit(i32 noundef %0) #0 {
  br label %2

2:                                                ; preds = %6, %1
  %.01 = phi i32 [ 0, %1 ], [ %5, %6 ]
  %.0 = phi i32 [ 0, %1 ], [ %7, %6 ]
  %3 = icmp slt i32 %.0, %0
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = add nsw i32 %.01, %.0
  br label %6

6:                                                ; preds = %4
  %7 = add nsw i32 %.0, 1
  br label %2, !llvm.loop !6

8:                                                ; preds = %2
  ret i32 %.01
}

; Function Attrs: noinline nounwind sspstrong uwtable
define dso_local i32 @multi_phi_cascade(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp ne i32 %0, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %6

5:                                                ; preds = %2
  br label %6

6:                                                ; preds = %5, %4
  %.01 = phi i32 [ 10, %4 ], [ 20, %5 ]
  %7 = icmp ne i32 %1, 0
  br i1 %7, label %8, label %10

8:                                                ; preds = %6
  %9 = add nsw i32 20, 1
  br label %12

10:                                               ; preds = %6
  %11 = add nsw i32 20, 2
  br label %12

12:                                               ; preds = %10, %8
  %.0 = phi i32 [ 21, %8 ], [ 22, %10 ]
  ret i32 %.0
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
