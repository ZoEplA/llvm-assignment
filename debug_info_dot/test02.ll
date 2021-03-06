; ModuleID = './debug_info_dot/test02.c'
source_filename = "./debug_info_dot/test02.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @plus(i32 %a, i32 %b) #0 !dbg !9 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !14, metadata !DIExpression()), !dbg !15
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !16, metadata !DIExpression()), !dbg !17
  %0 = load i32, i32* %a.addr, align 4, !dbg !18
  %1 = load i32, i32* %b.addr, align 4, !dbg !19
  %add = add nsw i32 %0, %1, !dbg !20
  ret i32 %add, !dbg !21
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @minus(i32 %a, i32 %b) #0 !dbg !22 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %a.addr, metadata !23, metadata !DIExpression()), !dbg !24
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %b.addr, metadata !25, metadata !DIExpression()), !dbg !26
  %0 = load i32, i32* %a.addr, align 4, !dbg !27
  %1 = load i32, i32* %b.addr, align 4, !dbg !28
  %sub = sub nsw i32 %0, %1, !dbg !29
  ret i32 %sub, !dbg !30
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @clever(i32 %x) #0 !dbg !31 {
entry:
  %x.addr = alloca i32, align 4
  %a_fptr = alloca i32 (i32, i32)*, align 8
  %s_fptr = alloca i32 (i32, i32)*, align 8
  %t_fptr = alloca i32 (i32, i32)*, align 8
  %op1 = alloca i32, align 4
  %op2 = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 %x, i32* %x.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %x.addr, metadata !34, metadata !DIExpression()), !dbg !35
  call void @llvm.dbg.declare(metadata i32 (i32, i32)** %a_fptr, metadata !36, metadata !DIExpression()), !dbg !38
  store i32 (i32, i32)* @plus, i32 (i32, i32)** %a_fptr, align 8, !dbg !38
  call void @llvm.dbg.declare(metadata i32 (i32, i32)** %s_fptr, metadata !39, metadata !DIExpression()), !dbg !40
  store i32 (i32, i32)* @minus, i32 (i32, i32)** %s_fptr, align 8, !dbg !40
  call void @llvm.dbg.declare(metadata i32 (i32, i32)** %t_fptr, metadata !41, metadata !DIExpression()), !dbg !42
  store i32 (i32, i32)* null, i32 (i32, i32)** %t_fptr, align 8, !dbg !42
  call void @llvm.dbg.declare(metadata i32* %op1, metadata !43, metadata !DIExpression()), !dbg !44
  store i32 1, i32* %op1, align 4, !dbg !44
  call void @llvm.dbg.declare(metadata i32* %op2, metadata !45, metadata !DIExpression()), !dbg !46
  store i32 2, i32* %op2, align 4, !dbg !46
  %0 = load i32, i32* %x.addr, align 4, !dbg !47
  %cmp = icmp eq i32 %0, 3, !dbg !49
  br i1 %cmp, label %if.then, label %if.else, !dbg !50

if.then:                                          ; preds = %entry
  %1 = load i32 (i32, i32)*, i32 (i32, i32)** %a_fptr, align 8, !dbg !51
  store i32 (i32, i32)* %1, i32 (i32, i32)** %t_fptr, align 8, !dbg !53
  br label %if.end, !dbg !54

if.else:                                          ; preds = %entry
  %2 = load i32 (i32, i32)*, i32 (i32, i32)** %s_fptr, align 8, !dbg !55
  store i32 (i32, i32)* %2, i32 (i32, i32)** %t_fptr, align 8, !dbg !57
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %3 = load i32 (i32, i32)*, i32 (i32, i32)** %t_fptr, align 8, !dbg !58
  %cmp1 = icmp ne i32 (i32, i32)* %3, null, !dbg !60
  br i1 %cmp1, label %if.then2, label %if.end3, !dbg !61

if.then2:                                         ; preds = %if.end
  call void @llvm.dbg.declare(metadata i32* %result, metadata !62, metadata !DIExpression()), !dbg !65
  %4 = load i32 (i32, i32)*, i32 (i32, i32)** %t_fptr, align 8, !dbg !66
  %5 = load i32, i32* %op1, align 4, !dbg !67
  %6 = load i32, i32* %op2, align 4, !dbg !68
  %call = call i32 %4(i32 %5, i32 %6), !dbg !66
  store i32 %call, i32* %result, align 4, !dbg !65
  br label %if.end3, !dbg !69

if.end3:                                          ; preds = %if.then2, %if.end
  ret i32 0, !dbg !70
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!5, !6, !7}
!llvm.ident = !{!8}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 10.0.0 ", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "debug_info_dot/test02.c", directory: "/home/zoe/Desktop/llvm-assignment")
!2 = !{}
!3 = !{!4}
!4 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!5 = !{i32 7, !"Dwarf Version", i32 4}
!6 = !{i32 2, !"Debug Info Version", i32 3}
!7 = !{i32 1, !"wchar_size", i32 4}
!8 = !{!"clang version 10.0.0 "}
!9 = distinct !DISubprogram(name: "plus", scope: !10, file: !10, line: 2, type: !11, scopeLine: 2, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DIFile(filename: "./debug_info_dot/test02.c", directory: "/home/zoe/Desktop/llvm-assignment")
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !DILocalVariable(name: "a", arg: 1, scope: !9, file: !10, line: 2, type: !13)
!15 = !DILocation(line: 2, column: 14, scope: !9)
!16 = !DILocalVariable(name: "b", arg: 2, scope: !9, file: !10, line: 2, type: !13)
!17 = !DILocation(line: 2, column: 21, scope: !9)
!18 = !DILocation(line: 3, column: 11, scope: !9)
!19 = !DILocation(line: 3, column: 13, scope: !9)
!20 = !DILocation(line: 3, column: 12, scope: !9)
!21 = !DILocation(line: 3, column: 4, scope: !9)
!22 = distinct !DISubprogram(name: "minus", scope: !10, file: !10, line: 6, type: !11, scopeLine: 6, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!23 = !DILocalVariable(name: "a", arg: 1, scope: !22, file: !10, line: 6, type: !13)
!24 = !DILocation(line: 6, column: 15, scope: !22)
!25 = !DILocalVariable(name: "b", arg: 2, scope: !22, file: !10, line: 6, type: !13)
!26 = !DILocation(line: 6, column: 22, scope: !22)
!27 = !DILocation(line: 7, column: 11, scope: !22)
!28 = !DILocation(line: 7, column: 13, scope: !22)
!29 = !DILocation(line: 7, column: 12, scope: !22)
!30 = !DILocation(line: 7, column: 4, scope: !22)
!31 = distinct !DISubprogram(name: "clever", scope: !10, file: !10, line: 10, type: !32, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!32 = !DISubroutineType(types: !33)
!33 = !{!13, !13}
!34 = !DILocalVariable(name: "x", arg: 1, scope: !31, file: !10, line: 10, type: !13)
!35 = !DILocation(line: 10, column: 16, scope: !31)
!36 = !DILocalVariable(name: "a_fptr", scope: !31, file: !10, line: 11, type: !37)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64)
!38 = !DILocation(line: 11, column: 11, scope: !31)
!39 = !DILocalVariable(name: "s_fptr", scope: !31, file: !10, line: 12, type: !37)
!40 = !DILocation(line: 12, column: 11, scope: !31)
!41 = !DILocalVariable(name: "t_fptr", scope: !31, file: !10, line: 13, type: !37)
!42 = !DILocation(line: 13, column: 11, scope: !31)
!43 = !DILocalVariable(name: "op1", scope: !31, file: !10, line: 15, type: !13)
!44 = !DILocation(line: 15, column: 9, scope: !31)
!45 = !DILocalVariable(name: "op2", scope: !31, file: !10, line: 15, type: !13)
!46 = !DILocation(line: 15, column: 16, scope: !31)
!47 = !DILocation(line: 17, column: 9, scope: !48)
!48 = distinct !DILexicalBlock(scope: !31, file: !10, line: 17, column: 9)
!49 = !DILocation(line: 17, column: 11, scope: !48)
!50 = !DILocation(line: 17, column: 9, scope: !31)
!51 = !DILocation(line: 18, column: 17, scope: !52)
!52 = distinct !DILexicalBlock(scope: !48, file: !10, line: 17, column: 17)
!53 = !DILocation(line: 18, column: 15, scope: !52)
!54 = !DILocation(line: 19, column: 5, scope: !52)
!55 = !DILocation(line: 20, column: 17, scope: !56)
!56 = distinct !DILexicalBlock(scope: !48, file: !10, line: 19, column: 12)
!57 = !DILocation(line: 20, column: 15, scope: !56)
!58 = !DILocation(line: 23, column: 9, scope: !59)
!59 = distinct !DILexicalBlock(scope: !31, file: !10, line: 23, column: 9)
!60 = !DILocation(line: 23, column: 16, scope: !59)
!61 = !DILocation(line: 23, column: 9, scope: !31)
!62 = !DILocalVariable(name: "result", scope: !63, file: !10, line: 24, type: !64)
!63 = distinct !DILexicalBlock(scope: !59, file: !10, line: 23, column: 25)
!64 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!65 = !DILocation(line: 24, column: 17, scope: !63)
!66 = !DILocation(line: 24, column: 26, scope: !63)
!67 = !DILocation(line: 24, column: 33, scope: !63)
!68 = !DILocation(line: 24, column: 38, scope: !63)
!69 = !DILocation(line: 25, column: 5, scope: !63)
!70 = !DILocation(line: 26, column: 4, scope: !31)
