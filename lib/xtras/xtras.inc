;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xtras - ChrysaLisp extra forms
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Predicates

(defun-bind neg? (el)
  ; (neg? num) -> bool
  (< el 0))

(defun-bind pos? (el)
  ; (pos? num) -> bool
  (> el 0))

(defun-bind zero? (el)
  ; (zero? num) -> bool
  (= el 0))

(defun-bind one? (el)
  ; (one? num) -> bool
  (= el 1))

(defun-bind odd? (el)
  ; (odd? num) -> bool
  (one? (logand 1 el)))

(defun-bind even? (el)
  ; (even? num) -> bool
  (not (odd? el)))

; Property (a.k.a. dictionary a.k.a. map) forms

(defun-bind dict-property-list (_)
  ; (dict-property-list x) -> list
  ; Returns list of properties for dictionary
  (tolist _))

(defmacro-bind make-dict (name &rest pairs)
  ; (make-dict sym [:key val...]) -> dict
  ; Creates a new dictionary with optional property value pairs
  (if (> (length pairs) 0)
    `(progn
      (defq ,name (env -16)) (def ,name ~pairs) ,name)
    `(defq ,name (env -16))))

(defun-bind dict-get (dict property &optional ifnil)
  ; (dict-get dict property &optional ifnil) -> nil | ifnil
  ; Returns a property or optional ifnil value
  (defq tmp (get property dict))
  (when (not tmp)
    (setq tmp ifnil))
  tmp)

(defun-bind dict-drop! (dict &rest keys)
  ; (dict-drop! dict key...) -> dict
  (each (#(undef dict %0)) keys)
  dict)

(defun-bind dict-put! (dict property value &optional add_ifnil)
  ; (dict-put dict property value &optional add_ifnil) -> dict
  ; Updates the property value of a dictionary
  (if (not (get property dict))
    (if add_ifnil
      (def dict property value))
    (set dict property value))
  dict)

(defun-bind dict-merge (&rest dlist)
  ; (dict-merge dict0 ...) -> list
  ; Merges multiple dictionaries to a list of property lists
  (defq
    ebase (dict-property-list (first dlist))
    kset  (list))
  (each (lambda (el) (push (push kset (first el)) _)) ebase)
  (each (lambda (el)
    (reduce (lambda (acc el)
      (defq hit (find (first el) kset))
      (cond
        (hit
          (elem-set 1 (elem (elem (inc hit) kset) acc) (second el))
          acc)
        (t
          (push (push kset (first el)) (/ (length kset) 2))
          (push acc el)))) el ebase)) (map dict-property-list (rest dlist)))
  ebase)

(defun-bind dict-merge-to! (dict &rest dlist)
  ; (dict-merge-to dict dict0 ...) -> dict
  ; Merges multiple dictionaries to a dictionary
  (reduce (lambda (acc el) (def acc (first el) (second el)) acc)
    (apply dict-merge dlist) dict))

(defun-bind dict-keys (dict)
  ; (dict-keys dict) -> key list
  (map (#(elem 0 %0)) (dict-property-list dict)))

(defun-bind dict-values (dict)
  ; (dict-values dict) -> value list
  (map (#(elem 1 %0)) (dict-property-list dict)))

(defun-bind dict? (_)
  (env? _))

; Test cases
; (make-dict foo :a 0 :b 1) ; -> ((:a 0) (:b 1))
; (make-dict bar :a 2 :c 3) ; -> ((:a 2) (:c 3))
; (make-dict baz :d 5 :c 7) ; -> ((:d 5) (:c 7))
; (dict-merge foo bar baz)  ; -> ((:b 1) (:a 2) (:c 7) (:d 5))
; (apply dict-merge (list foo bar baz))  ; -> ((:b 1) (:a 2) (:c 7) (:d 5))
; (dict-merge-to! (make-dict bingo) foo bar baz) ; -> ((:d 5) (:b 1) (:a 2) (:c 7))
; (dict-drop! bingo :d :c)

; Walkers

(defun-bind id (_)_)

(defun walk-tree (tree fnc)
  (defq stack (if (> (length tree) 0) (list tree 0) '()))
  (while (defq i (pop stack) l (pop stack))
    (defq e (elem i l))
    (if (< (setq i (inc i)) (length l))
        (push stack l i))
    (if (lst? e)
      (if (> (length e) 0) (push stack e 0))
      (fnc e))))

(defun-bind walk-tree-recur (inner-fn outer-fn tree)
  ; (walk-tree-recur inner-fn outer-fn tree) -> tree
  ; Recusively walk tree and handles dictionaries
  (cond
    ((lst? tree)
      (outer-fn
        (apply list
          (map (lambda (el)
            (if (dict? el)
              (walk-tree-recur inner-fn outer-fn el)
              (inner-fn el))) tree))))
    ((dict? tree)
      (map (curry walk-tree-recur inner-fn outer-fn) (dict-property-list tree)))
    (t (outer-fn tree))))

(defun-bind pre-walk-recur (fn tree)
  ; (pre-walk-recur fn tree)
  (walk-tree-recur (curry pre-walk-recur fn) id (fn tree)))

(defun-bind post-walk-recur (fn tree)
  ; (post-walk-recur fn tree)
  (walk-tree-recur (curry post-walk-recur fn) fn tree))


; Test utilities
; (defun visit (_)
;   (print "Visiting " _)
;   _)

; (defun outf (_)
;   (print "Outter = " _)
;   _)
; (defun inf (_)
;   (print "Inner = " _)
;   _)

; (defq t0 (list 0 1 2 3 (make-dict foo :a 7 :b 6) (list 0 1 2 (list 3 4 5))))
; (defq t1 (list 0 1 2 3 (list 0 1 2 (list 3 4 5))))
; (defq t2 (list 0 1 2 (make-dict foo :a 1 :b 7) (list 0 1 2 (list 3 4 5))))
; (make-dict bar :a 0 :b 1 :c (list foo))

; Take and drop forms

(defun-bind take (cnt seq)
  ; (take cnt seq) -> seq
  ; Returns the first `cnt` of elements from seq
  ; providing a negative cnt returns the original sequence
  ; example: (take 2 '(0 1 2 3)) -> (0 1)
  (defq len (length seq))
  (cond
    ((neg? cnt) seq)
    (t (slice 0 (cond ((> cnt len) len) (t cnt)) seq))))


(defun-bind take-last (cnt seq)
  ; (take-last cnt seq) -> seq
  ; Returns the last 'cnt' of elements from seq
  ; providing a negative cnt returns the original sequence
  ; example: (take-last 2 '(0 1 2 3)) ->  (2 3)
  (reverse (take cnt (reverse seq))))

(defun-bind take-while (pred seq)
  ; (take-while pred seq)
  ; Returns successive element of seq while pred returns true
  ; example: (take-while neg? '(-1 -2 0 -1 -2)) -> (-1 -2)
  (reduced-reduce
    (lambda (acc el)
      (cond
        ((pred el) (push acc el))
        (t (reduced acc))))
    seq (list)))

(defun-bind take-unless (pred seq)
  ; (take-unless pred seq)
  ; Returns successive element of seq unless pred returns true
  ; example: (take-unless (# (< %0 0)) '(-1 -2 0 -1 -2)) -> (0)
  (reduced-reduce
    (lambda (acc el)
      (cond
        ((pred el) acc)
        (t (push acc el))))
    seq (list)))

(defun-bind drop (cnt seq)
  ; (drop cnt seq) -> seq
  ; Returns a sequence with the first `cnt` of elements removed
  ; providing a negative cnt returns the original sequence
  (defq len (length seq))
  (cond
    ((neg? cnt) seq)
    (t (slice (cond ((> cnt len) -1) (t cnt)) -1 seq))))

(defun-bind drop-last (cnt seq)
  ; (drop-last cnt seq) -> seq
  ; providing a negative cnt returns the original sequence
  (reverse (drop cnt (reverse seq))))

(defun-bind drop-while (pred seq)
  ; (drop-while pred seq)
  ; Drops successive element of seq while pred returns true
  (slice (length (take-while pred seq)) -1 seq))

(defun-bind drop-unless (pred seq)
  (reduced-reduce
    (lambda (acc el)
      (cond
        ((pred el) (push acc el))
        (t acc)))
    seq (list)))

; Test cases
; (defq myL2 (list -2 -1 0 -1 -2 3))
; (take 2 myL2)       ; -> (-2 -1)
; (take-last 2 myL2)  ; -> (-2 3)
; (take-while (# (< %0 0)) myL2)  ; -> (-2 -1)
; (take-unless (# (< %0 0)) myL2) ; -> (0 3)
; (drop 2 myL2)       ; -> (0 -1 -2 3)
; (drop-last 2 myL2)  ; -> (-2 -1 0 -1)
; (drop-while (# (< %0 0)) myL2)  ; -> (0 -1 -2 3)
; (drop-unless (# (< %0 0)) myL2) ; -> (-2 -1 -1 -2)

