/**
 * Author: rodrigo
 * 2016
 */
#include "DescriptorXX.hpp"
#include <pcl/features/shot.h>
#include <pcl/common/io.h>
#include "CloudUtils.hpp"


void SHOT::computeDense(const pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud_,
						const DescriptorParamsPtr &params_,
						cv::Mat &descriptors_) const
{
	// Remove any NaN
	pcl::PointCloud<pcl::PointXYZ>::Ptr filtered(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::copyPointCloud(*cloud_, *filtered);
	CloudUtils::removeNANs(filtered);

	// Estimate normals
	pcl::PointCloud<pcl::Normal>::Ptr normals = CloudUtils::estimateNormals(filtered, -1);

	// Compute the descriptor
	SHOTParams *params = (SHOTParams *)params_.get();
	pcl::PointCloud<pcl::SHOT352>::Ptr descCloud(new pcl::PointCloud<pcl::SHOT352>());
	pcl::SHOTEstimation<pcl::PointXYZ, pcl::Normal, pcl::SHOT352> shot;
	shot.setInputCloud(filtered);
	shot.setInputNormals(normals);
	shot.setRadiusSearch(params->searchRadius);
	shot.setLRFRadius(params->searchRadius);
	shot.compute(*descCloud);

	// Prepare matrix to copy data
	int rows = descCloud->size();
	int cols = 352;
	if (descriptors_.rows != rows || descriptors_.cols != cols)
		descriptors_ = cv::Mat::zeros(rows, cols, CV_32FC1);

	// Copy data to matrix
	for (int i = 0; i < rows; i++)
		memcpy(&descriptors_.at<float>(i), &descCloud->at(i).descriptor, cols);
}