
(cl:in-package :asdf)

(defsystem "semantic_label_publisher-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "SemLabel" :depends-on ("_package_SemLabel"))
    (:file "_package_SemLabel" :depends-on ("_package"))
  ))