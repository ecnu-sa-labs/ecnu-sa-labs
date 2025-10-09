; ModuleID = 'test1.c'
source_filename = "test1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  tail call void @llvm.dbg.declare(metadata ptr %a, metadata !15, metadata !DIExpression()), !dbg !16
  %call = call i32 @getchar(), !dbg !17
  store i32 %call, ptr %a, align 4, !dbg !16
  tail call void @llvm.dbg.declare(metadata ptr %b, metadata !18, metadata !DIExpression()), !dbg !19
  store i32 -5, ptr %b, align 4, !dbg !19
  tail call void @llvm.dbg.declare(metadata ptr %c, metadata !20, metadata !DIExpression()), !dbg !21
  %0 = load i32, ptr %a, align 4, !dbg !22
  %1 = load i32, ptr %b, align 4, !dbg !23
  %cmp = icmp ne i32 %0, %1, !dbg !24
  %conv = zext i1 %cmp to i32, !dbg !24
  store i32 %conv, ptr %c, align 4, !dbg !21
  tail call void @llvm.dbg.declare(metadata ptr %d, metadata !25, metadata !DIExpression()), !dbg !26
  %2 = load i32, ptr %b, align 4, !dbg !27
  %3 = load i32, ptr %c, align 4, !dbg !28
  %div = sdiv i32 %2, %3, !dbg !29
  store i32 %div, ptr %d, align 4, !dbg !26
  ret i32 0, !dbg !30
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
!1 = !DIFile(filename: "test1.c", directory: "/lab2/c_programs", checksumkind: CSK_MD5, checksum: "16f5bdcadc2b986b92fb2e6831d953e7")
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
!18 = !DILocalVariable(name: "b", scope: !10, file: !1, line: 5, type: !13)
!19 = !DILocation(line: 5, column: 7, scope: !10)
!20 = !DILocalVariable(name: "c", scope: !10, file: !1, line: 6, type: !13)
!21 = !DILocation(line: 6, column: 7, scope: !10)
!22 = !DILocation(line: 6, column: 11, scope: !10)
!23 = !DILocation(line: 6, column: 16, scope: !10)
!24 = !DILocation(line: 6, column: 13, scope: !10)
!25 = !DILocalVariable(name: "d", scope: !10, file: !1, line: 7, type: !13)
!26 = !DILocation(line: 7, column: 7, scope: !10)
!27 = !DILocation(line: 7, column: 11, scope: !10)
!28 = !DILocation(line: 7, column: 15, scope: !10)
!29 = !DILocation(line: 7, column: 13, scope: !10)
!30 = !DILocation(line: 8, column: 3, scope: !10)
