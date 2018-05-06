// Auto-generated. Do not edit!

// (in-package mapper.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class SemLabel {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.label = null;
      this.r = null;
      this.g = null;
      this.b = null;
      this.prob = null;
      this.lvl = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('label')) {
        this.label = initObj.label
      }
      else {
        this.label = '';
      }
      if (initObj.hasOwnProperty('r')) {
        this.r = initObj.r
      }
      else {
        this.r = [];
      }
      if (initObj.hasOwnProperty('g')) {
        this.g = initObj.g
      }
      else {
        this.g = [];
      }
      if (initObj.hasOwnProperty('b')) {
        this.b = initObj.b
      }
      else {
        this.b = [];
      }
      if (initObj.hasOwnProperty('prob')) {
        this.prob = initObj.prob
      }
      else {
        this.prob = [];
      }
      if (initObj.hasOwnProperty('lvl')) {
        this.lvl = initObj.lvl
      }
      else {
        this.lvl = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type SemLabel
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [label]
    bufferOffset = _serializer.string(obj.label, buffer, bufferOffset);
    // Serialize message field [r]
    bufferOffset = _arraySerializer.int32(obj.r, buffer, bufferOffset, null);
    // Serialize message field [g]
    bufferOffset = _arraySerializer.int32(obj.g, buffer, bufferOffset, null);
    // Serialize message field [b]
    bufferOffset = _arraySerializer.int32(obj.b, buffer, bufferOffset, null);
    // Serialize message field [prob]
    bufferOffset = _arraySerializer.float32(obj.prob, buffer, bufferOffset, null);
    // Serialize message field [lvl]
    bufferOffset = _serializer.int32(obj.lvl, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type SemLabel
    let len;
    let data = new SemLabel(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [label]
    data.label = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [r]
    data.r = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [g]
    data.g = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [b]
    data.b = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [prob]
    data.prob = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [lvl]
    data.lvl = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    length += object.label.length;
    length += 4 * object.r.length;
    length += 4 * object.g.length;
    length += 4 * object.b.length;
    length += 4 * object.prob.length;
    return length + 24;
  }

  static datatype() {
    // Returns string type for a message object
    return 'mapper/SemLabel';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '0419e48e313c70a6c29f5da83e48665d';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    Header header
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
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new SemLabel(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.label !== undefined) {
      resolved.label = msg.label;
    }
    else {
      resolved.label = ''
    }

    if (msg.r !== undefined) {
      resolved.r = msg.r;
    }
    else {
      resolved.r = []
    }

    if (msg.g !== undefined) {
      resolved.g = msg.g;
    }
    else {
      resolved.g = []
    }

    if (msg.b !== undefined) {
      resolved.b = msg.b;
    }
    else {
      resolved.b = []
    }

    if (msg.prob !== undefined) {
      resolved.prob = msg.prob;
    }
    else {
      resolved.prob = []
    }

    if (msg.lvl !== undefined) {
      resolved.lvl = msg.lvl;
    }
    else {
      resolved.lvl = 0
    }

    return resolved;
    }
};

module.exports = SemLabel;
