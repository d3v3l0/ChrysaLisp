%include 'inc/func.inc'
%include 'inc/gui.inc'
%include 'class/class_title.inc'

	fn_function class/title/draw
		;inputs
		;r0 = view object
		;r1 = ctx object
		;trashes
		;all but r4

		vp_cpy 1, r2
		vp_cpy 1, r3
		static_call title, draw_panel
		vp_ret

	fn_function_end
