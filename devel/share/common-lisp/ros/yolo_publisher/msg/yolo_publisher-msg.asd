
(cl:in-package :asdf)

(defsystem "yolo_publisher-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "ObjLabel" :depends-on ("_package_ObjLabel"))
    (:file "_package_ObjLabel" :depends-on ("_package"))
  ))