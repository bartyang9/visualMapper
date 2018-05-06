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

#include "mapper/SemLabel.h"
#include <math.h>


#include <sensor_msgs/PointCloud2.h>

//#include <sensor_msgs/point_cloud_conversion.h>
//#include <sensor_msgs/Image.h>

//#include "message_filters/cache.h"
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
//#include <laser_geometry/laser_geometry.h>


#include <tf/tf.h>
#include <tf/transform_listener.h>

//#include <pcl_conversions/pcl_conversions.h>
#include <pcl/PCLPointCloud2.h>
//#include <pcl/ros/conversions.h>
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


boost::mutex m_lock_;

//typedef pcl::PCLPointCloud2 PclCloud;
//typedef pcl::PCLPointCloud2::Ptr PclCloudPtr;
//typedef sensor_msgs::PointCloud2 RosCloud;
//typedef sensor_msgs::PointCloud2Ptr RosCloudPtr;


//typedef octomap::ColorOcTreeNode SemCell;
//typedef octomap::ColorOcTreeNode::Color CellColor;


////////////////////////////////////////////////////////////////////////
std::string mlabelIn;
std::string mcloudOut;
std::string mcloudIn;
//ros::NodeHandle nh_;
//ros::NodeHandle local_nh_;
//ros::Subscriber labelSub_;
//ros::Subscriber laserSub_;
ros::Publisher mcludPub;
//ros::Publisher oneLabel_pub_;
//sensor_msgs::Image image_;

tf::TransformListener *tf_listener;
//pcl::PointCloud<pcl::PointXYZRGB>::Ptr label_cloud_ptr_;
//laser_geometry::LaserProjection projector_;
//ros::Time label_time;

//message_filters::Cache<sensor_msgs::PointCloud2> cache;

octomap::ColorOcTree *semMap;
//double cell_resolution_;
//double sem_ProbHit_;
//double sem_ProbMiss_;
double prob_bar;

/////////////////////////////////////////////////////////////////////////

using namespace message_filters ;


void label_CB(const mapper::SemLabel::ConstPtr& msgPtr, const sensor_msgs::PointCloud2::ConstPtr& depth_scan){
    boost::mutex::scoped_lock l(m_lock_);
    std::cout<< "messgage received" << std::endl;
    mapper::SemLabel msg = *msgPtr;
    
    //label_time = msg.header.stamp;
    //sensor_msgs::PointCloud2::ConstPtr pc = cache.getElemBeforeTime(label_time);

    if ((depth_scan) && (msg.prob[msg.lvl] > prob_bar))
    {
        //std::cout << "prob" << msg.prob[msg.lvl] <<"\n";
        //std::cout << "lvl" << msg.lvl <<"\n";
        // pub_sem_cloud(pc,msg.prob,msg.lvl,msg.r,msg.g,msg.b);
        std::vector<float> prob = msg.prob;
        //std::cout << "depth received\n\\";
        pcl::PCLPointCloud2::Ptr source(new pcl::PCLPointCloud2());
        pcl_conversions::toPCL(*depth_scan,*source);
        pcl::PointCloud<pcl::PointXYZ>::Ptr uncoloredCloud (new pcl::PointCloud<pcl::PointXYZ>);
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr coloredCloud (new pcl::PointCloud<pcl::PointXYZRGB>);
        pcl::fromPCLPointCloud2(*source, *uncoloredCloud);

        for(pcl::PointCloud<pcl::PointXYZ>::iterator iter = uncoloredCloud->begin(); iter != uncoloredCloud->end(); iter++)
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
	                    coloredCloud->push_back(p);
	                }
	            }
            }
        }

        tf::StampedTransform sensor_tf;
        tf_listener->waitForTransform("map", "base_link",ros::Time::now(),ros::Duration(0.2));
        tf_listener->lookupTransform("map", "base_link",depth_scan->header.stamp, sensor_tf);

        //sensor_tf.getRotation;
        coloredCloud->sensor_origin_[0] = sensor_tf.getOrigin().x();
        coloredCloud->sensor_origin_[1] = sensor_tf.getOrigin().y();
        coloredCloud->sensor_origin_[2] = sensor_tf.getOrigin().z();
        coloredCloud->sensor_orientation_.z() = sensor_tf.getRotation().z();
        coloredCloud->sensor_orientation_.w() = sensor_tf.getRotation().w();
        coloredCloud->sensor_orientation_.x() = sensor_tf.getRotation().x();
        coloredCloud->sensor_orientation_.y() = sensor_tf.getRotation().y();

        Eigen::Matrix3f rotMatrix = coloredCloud->sensor_orientation_.toRotationMatrix();
        Eigen::Matrix4f t;
        t.block<3,3>(0,0) = rotMatrix;
        t.block<3,1>(0,3) = Eigen::Vector3f(coloredCloud->sensor_origin_[0],coloredCloud->sensor_origin_[1],coloredCloud->sensor_origin_[2]);
        t.bottomRows(1).setZero();
        t(3,3) = 1;
        // t(0, 0) = rotMatrix(0, 0);
        // t(0, 1) = rotMatrix(0, 1);
        // t(0, 2) = rotMatrix(0, 2);
        // t(1, 0) = rotMatrix(1, 0);
        // t(1, 1) = rotMatrix(1, 1);
        // t(1, 2) = rotMatrix(1, 2);
        // t(2, 0) = rotMatrix(2, 0);
        // t(2, 1) = rotMatrix(2, 1);
        // t(2, 2) = rotMatrix(2, 2);
        // t(3, 0) = t(3, 1) = t(3, 2) = 0;
        // t(3, 3) = 1;
        // t(0, 3) = coloredCloud->sensor_origin_[0];
        // t(1, 3) = coloredCloud->sensor_origin_[1];
        // t(2, 3) = coloredCloud->sensor_origin_[2];
        pcl::transformPointCloud(*coloredCloud, *coloredCloud, t);
        
        //Store the color inside the octamap cell with color label obtained from the pointcloud [Accumulate]
        for(pcl::PointCloud<pcl::PointXYZRGB>::const_iterator itera = coloredCloud->begin(); itera != coloredCloud->end(); itera++)
        {
            octomap::point3d point(itera->x, itera->y, itera->z);
            semMap->updateNode(point,true);
            octomap::ColorOcTreeNode* cell = semMap->search(point);
            octomap::ColorOcTreeNode::Color cellColor(itera->r,itera->g,itera->b);
            cell->setColor(cellColor);
        }

        // extract pointCloud from the octamap, and publish
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr pOut(new pcl::PointCloud<pcl::PointXYZRGB>());
        for(octomap::ColorOcTree::iterator itr = semMap->begin_leafs(), end = semMap->end_leafs(); itr != end; itr++)
        {
        	octomap::point3d temppoint = itr.getCoordinate();
            octomap::ColorOcTreeNode *cell = semMap->search(temppoint);
            octomap::ColorOcTreeNode::Color c = cell->getColor();
            pcl::PointXYZRGB pcl_point; 
            pcl_point.x = temppoint.x();
            pcl_point.y = temppoint.y();
            pcl_point.z = temppoint.z();
            pcl_point.r = c.r; 
            pcl_point.g = c.g; 
            pcl_point.b = c.b;
            pOut->push_back(pcl_point);
        }
        pcl::PCLPointCloud2::Ptr pclOut(new pcl::PCLPointCloud2());
        pcl::toPCLPointCloud2(*pOut,*pclOut);
        sensor_msgs::PointCloud2 outputCloud;
        pcl_conversions::fromPCL(*pclOut,outputCloud);
        outputCloud.header.frame_id = "map";
        outputCloud.header.stamp = depth_scan->header.stamp;
        mcludPub.publish(outputCloud);
    }
    /*else
    {
         std::cout << "no depth_scan found" << std::endl;
    }*/
}

// void Callback_laser(const sensor_msgs::PointCloud2::ConstPtr& depth_scan)
// {         
//             cache.add(depth_scan);
// }


int main(int argc, char** argv){
    ros::init(argc,argv,"mapper");
    ros::NodeHandle nh; 
    ros::NodeHandle local_nh("~");

    
    tf_listener = new tf::TransformListener(local_nh, ros::Duration(100));

    local_nh.param<std::string>("label_topic",mlabelIn,"/semantic_label");
    local_nh.param<std::string>("output_cloud",mcloudOut,"mapper/cloud");
    local_nh.param<std::string>("laser_topic",mcloudIn,"/camera/depth_registered/points");
    local_nh.param<double>("prob_thres",prob_bar,0.5);

    message_filters::Subscriber<mapper::SemLabel> mylabel(local_nh, mlabelIn, 1);
    message_filters::Subscriber<sensor_msgs::PointCloud2> mycloud(local_nh,mcloudIn , 1);
    typedef message_filters::sync_policies::ApproximateTime<mapper::SemLabel,sensor_msgs::PointCloud2> MySyncPolicy;
    // ApproximateTime takes a queue size as its constructor argument, hence MySyncPolicy(10)
    message_filters::Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), mylabel, mycloud);
    sync.registerCallback(boost::bind(&label_CB, _1, _2));
    
    mcludPub = nh.advertise<sensor_msgs::PointCloud2> (mcloudOut,1);

    
    semMap = new octomap::ColorOcTree(0.1);
    semMap->setClampingThresMax(1.0);
    semMap->setOccupancyThres(0);

    ros::spin();

}
