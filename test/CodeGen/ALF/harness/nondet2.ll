; ModuleID = '<stdin>'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@vint_init = global i32 9, align 4
@vint_ptr = global i32* @vint_init, align 8
@tmp = common global i32 0, align 4

define i32 @nondet_int() nounwind uwtable {
entry:
  %tmp = load i32** @vint_ptr, align 8, !tbaa !0
  %tmp1 = load volatile i32* %tmp, align 4, !tbaa !3
  ret i32 %tmp1
}

define i32 @f(i32 %i) nounwind uwtable readnone noinline {
entry:
  %add = add nsw i32 %i, 1
  ret i32 %add
}

define i32 @main() nounwind uwtable {
entry:
  %tmp = load i32** @vint_ptr, align 8, !tbaa !0
  %tmp1 = load volatile i32* %tmp, align 4, !tbaa !3
  %tmp2 = icmp ult i32 %tmp1, 10
  br i1 %tmp2, label %do.body, label %if.end

do.body:                                          ; preds = %do.body, %entry
  %i.0 = phi i32 [ %call2, %do.body ], [ %tmp1, %entry ]
  %call2 = tail call i32 @f(i32 %i.0)
  %cmp3 = icmp slt i32 %call2, 10
  br i1 %cmp3, label %do.body, label %if.end.loopexit

if.end.loopexit:                                  ; preds = %do.body
  store i32 %call2, i32* @tmp, align 4
  %phitmp = add i32 %call2, -10
  br label %if.end

if.end:                                           ; preds = %if.end.loopexit, %entry
  %i.1 = phi i32 [ 0, %entry ], [ %phitmp, %if.end.loopexit ]
  ret i32 %i.1
}

!0 = metadata !{metadata !"any pointer", metadata !1}
!1 = metadata !{metadata !"omnipotent char", metadata !2}
!2 = metadata !{metadata !"Simple C/C++ TBAA"}
!3 = metadata !{metadata !"int", metadata !1}
