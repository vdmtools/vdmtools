;; We assume that there is a path to vicede already.
(defvar vicede-command-name "vicede"
  "Commandname for executing vicede.")

;;
;; $Id: vicede.el,v 1.4 2005/12/20 06:08:36 vdmtools Exp $
;;

;; Run gdb under Emacs
;; Author: W. Schelter, University of Texas
;;     wfs@rascal.ics.utexas.edu
;; Rewritten by rms.
;; Some ideas are due to  Masanobu.

;; Customized for the vdm-sl debugger by Michael Andersen
;; at May 10, 1992

;; This file is covered by the GNU Emacs General Public License.
;; Refer to the GNU Emacs General Public License for full details.
;; It should be in a file named COPYING.
;; Copyright (C) 1988 Free Software Foundation, Inc.


(require 'gud)

;; ======================================================================
;; vicede functions

;;; History of argument lists passed to vicede.
(defvar gud-vicede-history nil)

(defun gud-vicede-massage-args (file args)
  (cons file args))

;; There's no guarantee that Emacs will hand the filter the entire
;; marker at once; it could be broken up across several strings.  We
;; might even receive a big chunk with several markers in it.  If we
;; receive a chunk of text which looks like it might contain the
;; beginning of a marker, we save it here between calls to the
;; filter.
(defvar gud-vicede-marker-acc "")

(defun gud-vicede-marker-filter (string)
  (save-match-data
    (setq gud-vicede-marker-acc (concat gud-vicede-marker-acc string))
    (let ((output ""))

      ;; Process all the complete markers in this chunk.
      (while (string-match "\032\032\\([^:\n]*\\):\\(-?[0-9]*\\):.*\n"
			   gud-vicede-marker-acc)
	(setq

	 ;; Extract the frame position from the marker.
	 gud-last-frame
	 (cons (substring gud-vicede-marker-acc (match-beginning 1) (match-end 1))
	       (string-to-int (substring gud-vicede-marker-acc
					 (match-beginning 2)
					 (match-end 2))))

	 ;; Append any text before the marker to the output we're going
	 ;; to return - we don't include the marker in this text.
	 output (concat output
			(substring gud-vicede-marker-acc 0 (match-beginning 0)))

	 ;; Set the accumulator to the remaining text.
	 gud-vicede-marker-acc (substring gud-vicede-marker-acc (match-end 0))))

      ;; Does the remaining text look like it might end with the
      ;; beginning of another marker?  If it does, then keep it in
      ;; gud-vicede-marker-acc until we receive the rest of it.  Since we
      ;; know the full marker regexp above failed, it's pretty simple to
      ;; test for marker starts.
      (if (string-match "\032.*\\'" gud-vicede-marker-acc)
	  (progn
	    ;; Everything before the potential marker start can be output.
	    (setq output (concat output (substring gud-vicede-marker-acc
						   0 (match-beginning 0))))

	    ;; Everything after, we save, to combine with later input.
	    (setq gud-vicede-marker-acc
		  (substring gud-vicede-marker-acc (match-beginning 0))))

	(setq output (concat output gud-vicede-marker-acc)
	      gud-vicede-marker-acc ""))

      output)))

(defun gud-vicede-find-file (f)
  (find-file-noselect f))


;;;###autoload
(defun vicede (command-line)
  "Run vicede on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run vicede (like this): "
			       (if (consp gud-vicede-history)
				   (car gud-vicede-history)
				 (concat vicede-command-name " "))
			       nil nil
			       '(gud-vicede-history . 1))))

  ;; From 19.29 the gud overloading interface was changed. /etn
  ;; emacs-major-version and emacs-minor-version existed first in
  ;; emacs 19.23, so we must check if the vars are defined.
  ;; XEmacs 19.13 takes a second parameter to gud-common-init
;;  (cond ((string-match "XEmacs" emacs-version)
;;         (gud-overload-functions '((gud-massage-args . gud-vicede-massage-args)
;;                                   (gud-marker-filter . gud-vicede-marker-filter)
;;                                   (gud-find-file . gud-vicede-find-file)
;;                                   ))
;;         (gud-common-init command-line "vicede"))
;;        ;; GNU Emacs 19.29+ ?
;;        ((and (boundp 'emacs-major-version)
;;              (boundp 'emacs-minor-version)
;;              (or (and (= emacs-major-version 19)
;;                       (> emacs-minor-version 28)
;;                       )
;;                  (= emacs-major-version 20)
;;                        ))
;;         (gud-common-init command-line 'gud-vicede-massage-args
;;                          'gud-vicede-marker-filter 'gud-vicede-find-file))
;;
;;        (t
;;         ;; Assume GNU Emacs 19.29-
;;         (gud-overload-functions '((gud-massage-args . gud-vicede-massage-args)
;;                                   (gud-marker-filter . gud-vicede-marker-filter)
;;                                   (gud-find-file . gud-vicede-find-file)
;;                                   ))
;;         (gud-common-init command-line )))

  ;; Assume GNU Emacs 19.29+
  (gud-common-init command-line 'gud-vicede-massage-args
                                'gud-vicede-marker-filter 'gud-vicede-find-file)

  (gud-def gud-break  "break %f:%l"  "\C-b" "Set breakpoint at current line.")
  (gud-def gud-tbreak "tbreak %f:%l" "\C-t" "Set breakpoint at current line.")
  (gud-def gud-remove "clear %l"     "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "step %p"      "\C-s" "Step one source line with display.")
  (gud-def gud-stepi  "stepi %p"     "\C-i" "Step one instruction with display.")
  (gud-def gud-next   "next %p"      "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "cont"         "\C-r" "Continue with display.")
  (gud-def gud-finish "finish"       "\C-f" "Finish executing current function.")
  (gud-def gud-up     "up %p"        "<" "Up N stack frames (numeric arg).")
  (gud-def gud-down   "down %p"      ">" "Down N stack frames (numeric arg).")
  (gud-def gud-print  "print %e"     "\C-p" "Evaluate C expression at point.")

  (setq comint-prompt-regexp "^[^>]*> *")
  (run-hooks 'vicede-mode-hook)
  )


