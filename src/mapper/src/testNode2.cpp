/**
Copyright (C) 2016, by 
Feras Dayoub (feras.dayoub@gmail.com) 

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This software package is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Leser General Public License.
If not, see <http://www.gnu.org/licenses/>.
**/

// whats this for??????????
#include "mapper/SemLabel.h"
#include <math.h>


#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud2.h>

#include <sensor_msgs/point_cloud_conversion.h>
#include <sensor_msgs/Image.h>

#include "message_filters/cache.h"
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <laser_geometry/laser_geometry.h>


#include <tf/tf.h>
#include <tf/transform_listener.h>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/ros/conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_ros/transforms.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/io/pcd_io.h>

#include <octomap/OcTree.h>
#include <octomap/OcTreeKey.h>
#include <octomap/octomap.h>
#include <octomap/ColorOcTree.h>
#include <octomap_ros/conversions.h>
#include <octomap/octomap_types.h>



#include <ros/ros.h>


//boost::mutex m_lock_;

typedef pcl::PCLPointCloud2 PclCloud;
typedef pcl::PCLPointCloud2::Ptr PclCloudPtr;
typedef sensor_msgs::PointCloud2 RosCloud;
typedef sensor_msgs::PointCloud2Ptr RosCloudPtr;


typedef octomap::ColorOcTreeNode SemCell;
typedef octomap::ColorOcTree SemMap;
typedef octomap::ColorOcTreeNode::Color CellColor;

using namespace message_filters ;

////////////////////////////////////////////////////////////////////////
std::string inputLabelTopic_;
std::string outputCloudTopic_;
std::string depthTopic_;
//ros::NodeHandle nh_;
//ros::NodeHandle local_nh_;
ros::Subscriber labelSub_;
ros::Subscriber laserSub_;
ros::Publisher cloudPub_;
ros::Publisher oneLabel_pub_;
sensor_msgs::Image image_;

tf::TransformListener *tf_listener_;
pcl::PointCloud<pcl::PointXYZRGB>::Ptr label_cloud_ptr_;
laser_geometry::LaserProjection projector_;
ros::Time label_time;

message_filters::Cache<sensor_msgs::PointCloud2> cache;

SemMap *semMap;
double cell_resolution_;
double sem_ProbHit_;
double sem_ProbMiss_;
double prob_thres_;

/////////////////////////////////////////////////////////////////////////



void label_CB(const mapper::SemLabel::ConstPtr& msgPtr, const sensor_msgs::PointCloud2::ConstPtr& depth_scan){
    //boost::mutex::scoped_lock l(m_lock_);
    std::cout<< "messgage received" << std::endl;
    mapper::SemLabel msg = *msgPtr;
    //label_time = msg.header.stamp;
    //sensor_msgs::PointCloud2::ConstPtr pc = cache.getElemBeforeTime(label_time);

    if ((depth_scan) && (msg.prob[msg.lvl] > prob_thres_))
    {
        //std::cout << "prob" << msg.prob[msg.lvl] <<"\n";
        //std::cout << "lvl" << msg.lvl <<"\n";
       // pub_sem_cloud(pc,msg.prob,msg.lvl,msg.r,msg.g,msg.b);
        std::vector<float> prob = msg.prob;
        std::cout << "depth received\n";
        PclCloudPtr source(new PclCloud());
        pcl_conversions::toPCL(*depth_scan,*source);
        pcl::PointCloud<pcl::PointXYZ>::Ptr raw_c (new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr filled_c (new pcl::PointCloud<pcl::PointXYZRGB>);
        pcl::fromPCLPointCloud2(*source, *raw_c);

        for( pcl::PointCloud<pcl::PointXYZ>::iterator iter = raw_c->begin(); iter != raw_c->end(); ++iter)
        {

            float d = std::sqrt(iter->x*iter->x + iter->z*iter->z);
            if(1 < std::atan2(iter->x, iter->z) < 2)
            {
            if (1 < d < 5){
                for (float i = 1/d; i < 1; i += 0.1) //sparse the point
                {
                    float maxprob = prob[0]; 
                    float maxprobindex = 0;
                    for (int l=0;l < prob.size();l++)
                    {
                        if(maxprob < prob[l])
                        {
                            maxprob = prob[l]; 
                            maxprobindex = l;
                        }
                    }
                    pcl::PointXYZRGB p;
                    p.x = iter->x * i;
                    p.y = iter->y * i;
                    p.z = iter->z * i;  
                    uint8_t r = 0, g =0, b = 0;
                    r = msg.r[maxprobindex];
                    g = msg.g[maxprobindex];
                    b = msg.b[maxprobindex];
                    uint32_t rgb = ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
                    p.rgb = *reinterpret_cast<float*>(&rgb);
                    filled_c->push_back(p);
                }

            }
            }
            
        }

        tf::StampedTransform sensor_tf;
        tf_listener_->waitForTransform("map", "base_link",ros::Time::now(),ros::Duration(0.2));
        tf_listener_->lookupTransform("map", "base_link",depth_scan->header.stamp, sensor_tf);

        //sensor_tf.getRotation;
        filled_c->sensor_origin_[0] = sensor_tf.getOrigin().x();
        filled_c->sensor_origin_[1] = sensor_tf.getOrigin().y();
        filled_c->sensor_origin_[2] = sensor_tf.getOrigin().z();
        filled_c->sensor_orientation_.z() = sensor_tf.getRotation().z();
        filled_c->sensor_orientation_.w() = sensor_tf.getRotation().w();
        filled_c->sensor_orientation_.x() = sensor_tf.getRotation().x();
        filled_c->sensor_orientation_.y() = sensor_tf.getRotation().y();

        Eigen::Matrix3f rotMatrix = filled_c->sensor_orientation_.toRotationMatrix();
        Eigen::Matrix4f t;
        t(0, 0) = rotMatrix(0, 0);
        t(0, 1) = rotMatrix(0, 1);
        t(0, 2) = rotMatrix(0, 2);
        t(1, 0) = rotMatrix(1, 0);
        t(1, 1) = rotMatrix(1, 1);
        t(1, 2) = rotMatrix(1, 2);
        t(2, 0) = rotMatrix(2, 0);
        t(2, 1) = rotMatrix(2, 1);
        t(2, 2) = rotMatrix(2, 2);

        t(3, 0) = t(3, 1) = t(3, 2) = 0;
        t(3, 3) = 1;
        t(0, 3) = filled_c->sensor_origin_[0];
        t(1, 3) = filled_c->sensor_origin_[1];
        t(2, 3) = filled_c->sensor_origin_[2];
        pcl::transformPointCloud(*filled_c, *filled_c, t);
        
     
        for(pcl::PointCloud<pcl::PointXYZRGB>::const_iterator it = filled_c->begin(); it != filled_c->end(); it++)
        {
            octomap::point3d point(it->x, it->y, it->z);
            semMap->updateNode(point,true);
            SemCell* cell = semMap->search(point);
            CellColor cS(it->r,it->g,it->b);
            cell->setColor(cS);
        }


        pcl::PointCloud<pcl::PointXYZRGB>::Ptr pOut(new pcl::PointCloud<pcl::PointXYZRGB>());
        for( SemMap::iterator itr = semMap->begin_leafs(), end = semMap->end_leafs(); itr != end; itr++)
        {
            SemCell *cell = semMap->search(itr.getCoordinate());
            pcl::PointXYZRGB pcl_point; 
            pcl_point.x = itr.getCoordinate().x();pcl_point.y = itr.getCoordinate().y();pcl_point.z = itr.getCoordinate().z();
            CellColor c = cell->getColor();
            pcl_point.r = c.r; pcl_point.g = c.g; pcl_point.b = c.b;
            pOut->push_back(pcl_point);
        }
        PclCloudPtr pclOut(new PclCloud());
        pcl::toPCLPointCloud2(*pOut,*pclOut);
        RosCloud outputCloud;
        pcl_conversions::fromPCL(*pclOut,outputCloud);
        outputCloud.header.frame_id = "map";
        outputCloud.header.stamp = depth_scan->header.stamp;
        cloudPub_.publish(outputCloud);
    }
    else
    {
         std::cout << "no depth_scan found" << "\n";
    }
}

// void Callback_laser(const sensor_msgs::PointCloud2::ConstPtr& depth_scan)
// {         
//             cache.add(depth_scan);
// }


int main(int argc, char** argv){
    ROS_INFO_ONCE("mapper started");
    ros::init(argc,argv,"mapper");
    ros::NodeHandle nh; 
    ros::NodeHandle lnh("~");

    
    tf_listener_ = new tf::TransformListener(lnh, ros::Duration(100));

    lnh.param<std::string>("label_topic",inputLabelTopic_,"/semantic_label");
    lnh.param<std::string>("output_cloud",outputCloudTopic_,"mapper/cloud");
    lnh.param<std::string>("laser_topic",depthTopic_,"/camera/depth_registered/points");
    lnh.param<double>("prob_thres",prob_thres_,0.5);

    message_filters::Subscriber<mapper::SemLabel> mylabel(lnh, inputLabelTopic_, 1);
    message_filters::Subscriber<sensor_msgs::PointCloud2> mycloud(lnh,depthTopic_ , 1);
    typedef message_filters::sync_policies::ApproximateTime<mapper::SemLabel,sensor_msgs::PointCloud2> MySyncPolicy;
    // ApproximateTime takes a queue size as its constructor argument, hence MySyncPolicy(10)
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(5), mylabel, mycloud);
    sync.registerCallback(boost::bind(&label_CB, _1, _2));
    
    cloudPub_ = nh.advertise<sensor_msgs::PointCloud2> (outputCloudTopic_,1);

    cell_resolution_ = 0.05;
    semMap = new SemMap(cell_resolution_);
    semMap->setClampingThresMax(1.0);
    semMap->setOccupancyThres(0);

    ros::spin();

}
