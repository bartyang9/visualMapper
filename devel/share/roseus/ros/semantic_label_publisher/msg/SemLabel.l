;; Auto-generated. Do not edit!


(when (boundp 'semantic_label_publisher::SemLabel)
  (if (not (find-package "SEMANTIC_LABEL_PUBLISHER"))
    (make-package "SEMANTIC_LABEL_PUBLISHER"))
  (shadow 'SemLabel (find-package "SEMANTIC_LABEL_PUBLISHER")))
(unless (find-package "SEMANTIC_LABEL_PUBLISHER::SEMLABEL")
  (make-package "SEMANTIC_LABEL_PUBLISHER::SEMLABEL"))

(in-package "ROS")
;;//! \htmlinclude SemLabel.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass semantic_label_publisher::SemLabel
  :super ros::object
  :slots (_header _label _r _g _b _prob _lvl ))

(defmethod semantic_label_publisher::SemLabel
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:label __label) "")
    ((:r __r) (make-array 0 :initial-element 0 :element-type :integer))
    ((:g __g) (make-array 0 :initial-element 0 :element-type :integer))
    ((:b __b) (make-array 0 :initial-element 0 :element-type :integer))
    ((:prob __prob) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:lvl __lvl) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _label (string __label))
   (setq _r __r)
   (setq _g __g)
   (setq _b __b)
   (setq _prob __prob)
   (setq _lvl (round __lvl))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:label
   (&optional __label)
   (if __label (setq _label __label)) _label)
  (:r
   (&optional __r)
   (if __r (setq _r __r)) _r)
  (:g
   (&optional __g)
   (if __g (setq _g __g)) _g)
  (:b
   (&optional __b)
   (if __b (setq _b __b)) _b)
  (:prob
   (&optional __prob)
   (if __prob (setq _prob __prob)) _prob)
  (:lvl
   (&optional __lvl)
   (if __lvl (setq _lvl __lvl)) _lvl)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; string _label
    4 (length _label)
    ;; int32[] _r
    (* 4    (length _r)) 4
    ;; int32[] _g
    (* 4    (length _g)) 4
    ;; int32[] _b
    (* 4    (length _b)) 4
    ;; float32[] _prob
    (* 4    (length _prob)) 4
    ;; int32 _lvl
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; string _label
       (write-long (length _label) s) (princ _label s)
     ;; int32[] _r
     (write-long (length _r) s)
     (dotimes (i (length _r))
       (write-long (elt _r i) s)
       )
     ;; int32[] _g
     (write-long (length _g) s)
     (dotimes (i (length _g))
       (write-long (elt _g i) s)
       )
     ;; int32[] _b
     (write-long (length _b) s)
     (dotimes (i (length _b))
       (write-long (elt _b i) s)
       )
     ;; float32[] _prob
     (write-long (length _prob) s)
     (dotimes (i (length _prob))
       (sys::poke (elt _prob i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; int32 _lvl
       (write-long _lvl s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; string _label
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _label (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; int32[] _r
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _r (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _r i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32[] _g
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _g (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _g i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32[] _b
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _b (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _b i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; float32[] _prob
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _prob (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _prob i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; int32 _lvl
     (setq _lvl (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get semantic_label_publisher::SemLabel :md5sum-) "0419e48e313c70a6c29f5da83e48665d")
(setf (get semantic_label_publisher::SemLabel :datatype-) "semantic_label_publisher/SemLabel")
(setf (get semantic_label_publisher::SemLabel :definition-)
      "Header header
string label
int32[] r
int32[] g
int32[] b
float32[] prob
int32 lvl


================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id

")



(provide :semantic_label_publisher/SemLabel "0419e48e313c70a6c29f5da83e48665d")


