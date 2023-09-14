; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 @getchar()
  store i32 %call, i32* %a, align 4
  %0 = load i32, i32* %a, align 4
  %cmp = icmp eq i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end12

if.then:                                          ; preds = %entry
  %call1 = call i32 @getchar()
  store i32 %call1, i32* %a, align 4
  %1 = load i32, i32* %a, align 4
  %cmp2 = icmp eq i32 %1, 3
  br i1 %cmp2, label %if.then3, label %if.end11

if.then3:                                         ; preds = %if.then
  %call4 = call i32 @getchar()
  store i32 %call4, i32* %a, align 4
  %2 = load i32, i32* %a, align 4
  %cmp5 = icmp eq i32 %2, 5
  br i1 %cmp5, label %if.then6, label %if.end10

if.then6:                                         ; preds = %if.then3
  %call7 = call i32 @getchar()
  store i32 %call7, i32* %a, align 4
  %3 = load i32, i32* %a, align 4
  %cmp8 = icmp eq i32 %3, 7
  br i1 %cmp8, label %if.then9, label %if.end

if.then9:                                         ; preds = %if.then6
  %4 = load i32, i32* %a, align 4
  %div = sdiv i32 1, %4
  store i32 %div, i32* %a, align 4
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

declare dso_local i32 @getchar() #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 8.0.1- (branches/release_80)"}
