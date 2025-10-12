; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  %call = call i32 @getchar()
  store i32 %call, ptr %a, align 4
  %0 = load i32, ptr %a, align 4
  %cmp = icmp eq i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end12

if.then:                                          ; preds = %entry
  %call1 = call i32 @getchar()
  store i32 %call1, ptr %a, align 4
  %1 = load i32, ptr %a, align 4
  %cmp2 = icmp eq i32 %1, 3
  br i1 %cmp2, label %if.then3, label %if.end11

if.then3:                                         ; preds = %if.then
  %call4 = call i32 @getchar()
  store i32 %call4, ptr %a, align 4
  %2 = load i32, ptr %a, align 4
  %cmp5 = icmp eq i32 %2, 5
  br i1 %cmp5, label %if.then6, label %if.end10

if.then6:                                         ; preds = %if.then3
  %call7 = call i32 @getchar()
  store i32 %call7, ptr %a, align 4
  %3 = load i32, ptr %a, align 4
  %cmp8 = icmp eq i32 %3, 7
  br i1 %cmp8, label %if.then9, label %if.end

if.then9:                                         ; preds = %if.then6
  %4 = load i32, ptr %a, align 4
  %div = sdiv i32 1, %4
  store i32 %div, ptr %a, align 4
  br label %if.end

if.end:                                           ; preds = %if.then9, %if.then6
  br label %if.end10

if.end10:                                         ; preds = %if.end, %if.then3
  br label %if.end11

if.end11:                                         ; preds = %if.end10, %if.then
  br label %if.end12

if.end12:                                         ; preds = %if.end11, %entry
  ret i32 0
}

declare i32 @getchar() #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 19.0.0 (++20240301064251+dd426fa5f931-1~exp1~20240301184412.1845)"}
