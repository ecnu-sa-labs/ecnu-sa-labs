; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  tail call void @llvm.dbg.declare(metadata ptr %a, metadata !15, metadata !DIExpression()), !dbg !16
  %call = call i32 @getchar(), !dbg !17
  store i32 %call, ptr %a, align 4, !dbg !16
  %0 = load i32, ptr %a, align 4, !dbg !18
  %cmp = icmp eq i32 %0, 1, !dbg !20
  br i1 %cmp, label %if.then, label %if.end12, !dbg !21

if.then:                                          ; preds = %entry
  %call1 = call i32 @getchar(), !dbg !22
  store i32 %call1, ptr %a, align 4, !dbg !24
  %1 = load i32, ptr %a, align 4, !dbg !25
  %cmp2 = icmp eq i32 %1, 3, !dbg !27
  br i1 %cmp2, label %if.then3, label %if.end11, !dbg !28

if.then3:                                         ; preds = %if.then
  %call4 = call i32 @getchar(), !dbg !29
  store i32 %call4, ptr %a, align 4, !dbg !31
  %2 = load i32, ptr %a, align 4, !dbg !32
  %cmp5 = icmp eq i32 %2, 5, !dbg !34
  br i1 %cmp5, label %if.then6, label %if.end10, !dbg !35

if.then6:                                         ; preds = %if.then3
  %call7 = call i32 @getchar(), !dbg !36
  store i32 %call7, ptr %a, align 4, !dbg !38
  %3 = load i32, ptr %a, align 4, !dbg !39
  %cmp8 = icmp eq i32 %3, 7, !dbg !41
  br i1 %cmp8, label %if.then9, label %if.end, !dbg !42

if.then9:                                         ; preds = %if.then6
  %4 = load i32, ptr %a, align 4, !dbg !43
  %div = sdiv i32 1, %4, !dbg !45
  store i32 %div, ptr %a, align 4, !dbg !46
  br label %if.end, !dbg !47

if.end:                                           ; preds = %if.then9, %if.then6
  br label %if.end10, !dbg !48

if.end10:                                         ; preds = %if.end, %if.then3
  br label %if.end11, !dbg !49

if.end11:                                         ; preds = %if.end10, %if.then
  br label %if.end12, !dbg !50

if.end12:                                         ; preds = %if.end11, %entry
  ret i32 0, !dbg !51
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @getchar() #2

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Ubuntu clang version 19.0.0 (++20240301064251+dd426fa5f931-1~exp1~20240301184412.1845)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "test4.c", directory: "/lab2/c_programs", checksumkind: CSK_MD5, checksum: "68b0f80d0bf950424bd76ede3a406c23")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Ubuntu clang version 19.0.0 (++20240301064251+dd426fa5f931-1~exp1~20240301184412.1845)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !11, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", scope: !10, file: !1, line: 4, type: !13)
!16 = !DILocation(line: 4, column: 7, scope: !10)
!17 = !DILocation(line: 4, column: 11, scope: !10)
!18 = !DILocation(line: 5, column: 7, scope: !19)
!19 = distinct !DILexicalBlock(scope: !10, file: !1, line: 5, column: 7)
!20 = !DILocation(line: 5, column: 9, scope: !19)
!21 = !DILocation(line: 5, column: 7, scope: !10)
!22 = !DILocation(line: 6, column: 9, scope: !23)
!23 = distinct !DILexicalBlock(scope: !19, file: !1, line: 5, column: 15)
!24 = !DILocation(line: 6, column: 7, scope: !23)
!25 = !DILocation(line: 7, column: 9, scope: !26)
!26 = distinct !DILexicalBlock(scope: !23, file: !1, line: 7, column: 9)
!27 = !DILocation(line: 7, column: 11, scope: !26)
!28 = !DILocation(line: 7, column: 9, scope: !23)
!29 = !DILocation(line: 8, column: 11, scope: !30)
!30 = distinct !DILexicalBlock(scope: !26, file: !1, line: 7, column: 17)
!31 = !DILocation(line: 8, column: 9, scope: !30)
!32 = !DILocation(line: 9, column: 11, scope: !33)
!33 = distinct !DILexicalBlock(scope: !30, file: !1, line: 9, column: 11)
!34 = !DILocation(line: 9, column: 13, scope: !33)
!35 = !DILocation(line: 9, column: 11, scope: !30)
!36 = !DILocation(line: 10, column: 13, scope: !37)
!37 = distinct !DILexicalBlock(scope: !33, file: !1, line: 9, column: 19)
!38 = !DILocation(line: 10, column: 11, scope: !37)
!39 = !DILocation(line: 11, column: 13, scope: !40)
!40 = distinct !DILexicalBlock(scope: !37, file: !1, line: 11, column: 13)
!41 = !DILocation(line: 11, column: 15, scope: !40)
!42 = !DILocation(line: 11, column: 13, scope: !37)
!43 = !DILocation(line: 12, column: 19, scope: !44)
!44 = distinct !DILexicalBlock(scope: !40, file: !1, line: 11, column: 21)
!45 = !DILocation(line: 12, column: 17, scope: !44)
!46 = !DILocation(line: 12, column: 13, scope: !44)
!47 = !DILocation(line: 13, column: 9, scope: !44)
!48 = !DILocation(line: 14, column: 7, scope: !37)
!49 = !DILocation(line: 15, column: 5, scope: !30)
!50 = !DILocation(line: 16, column: 3, scope: !23)
!51 = !DILocation(line: 17, column: 3, scope: !10)
