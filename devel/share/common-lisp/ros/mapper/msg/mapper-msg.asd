
(cl:in-package :asdf)

(defsystem "mapper-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "SemLabel" :depends-on ("_package_SemLabel"))
    (:file "_package_SemLabel" :depends-on ("_package"))
  ))