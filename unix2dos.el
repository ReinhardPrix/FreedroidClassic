(defun unix2dos () (interactive)
  (setq make-backup-files nil)
  (set-buffer-file-coding-system 'dos)
  (replace-regexp "." "")
  (save-buffer)
)






