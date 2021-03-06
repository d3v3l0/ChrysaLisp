(include 'sys/list/class.inc)
(include 'sys/heap/class.inc)
(include 'sys/mail/class.inc)

;;;;;;;;;;;;;;;;;;;
; task static class
;;;;;;;;;;;;;;;;;;;

(def-class 'sys_task)
(dec-method :statics 'sys/task/statics)
(dec-method :statics_init 'sys/task/statics_init)
(dec-method :tcb 'sys/task/tcb :static nil '(r0))
(dec-method :mailbox 'sys/task/mailbox :static nil '(r0))
(dec-method :callback 'sys/task/callback :static '(r0 r1))
(dec-method :start 'sys/task/start :static '(r0) '(r0 r1 r2))
(dec-method :stop 'sys/task/stop)
(dec-method :restore 'sys/task/restore)
(dec-method :count 'sys/task/count :static nil '(r0))
(dec-method :sleep 'sys/task/sleep :static '(r0))
(dec-method :suspend 'sys/task/suspend)
(dec-method :resume 'sys/task/resume :static '(r0) '(r0))
(dec-method :defer 'sys/task/defer :static '(r0))
(dec-method :set_priority 'sys/task/set_priority :static '(r0))
(dec-method :timer 'sys/task/timer :static nil '(r0))
(dec-method :open_child 'sys/task/open_child :static '(r0 r1) '(r0))
(dec-method :open_remote 'sys/task/open_remote :static '(r0 r1 r2) '(r0))
(dec-method :open_farm 'sys/task/open_farm :static '(r0 r1 r2) '(r0))
(dec-method :open_pipe 'sys/task/open_pipe :static '(r0) '(r0))
(dec-method :task_callback 'class/obj/null :static '(r0))

(dec-method :lisp_sleep 'sys/task/lisp_sleep :static '(r0 r1) '(r0 r1))
(dec-method :lisp_mailbox 'sys/task/lisp_mailbox :static '(r0 r1) '(r0 r1))
(dec-method :lisp_open_child 'sys/task/lisp_open_child :static '(r0 r1) '(r0 r1))
(dec-method :lisp_open_remote 'sys/task/lisp_open_remote :static '(r0 r1) '(r0 r1))
(dec-method :lisp_open_farm 'sys/task/lisp_open_farm :static '(r0 r1) '(r0 r1))
(dec-method :lisp_open_pipe 'sys/task/lisp_open_pipe :static '(r0 r1) '(r0 r1))

;;;;;;;;;;;;;;;;;
; task structures
;;;;;;;;;;;;;;;;;

(defcvar 'tk_stack_size (* 1024 4))

(def-struct 'tk_node 'ln_node)
	(ptr 'stack 'mbox_addr 'tls)
	(ulong 'time)
	(uint 'priority 'mbox_id)
(def-struct-end)

;number of distinct priorities
;currently experimenting with
;0 Kernel process and interupts
;1 Link Drivers and Postman process
;2 GUI process
;3 Applications
(defcvar 'num_task_lists 4)

(def-struct 'tk_statics)
	(ptr 'current_tcb 'kernel_tcb)
	(uint 'task_count 'cpu_total 'cpu_id)
	(local-align)
	;keep these together
	(struct 'timer_list 'lh_list)
	(struct 'task_lists (* num_task_lists lh_list_size))
(def-struct-end)

;;;;;;;;;;;;;
; task macros
;;;;;;;;;;;;;

(defcfun tk-save-state ()
	(apply vp-push stack_state))

(defcfun tk-load-state ()
	(apply vp-pop stack_state))
