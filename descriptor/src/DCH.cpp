/**
 * Author: rodrigo
 * 2015
 */
#include "DCH.hpp"
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <map>


using namespace boost::accumulators;


std::vector<BandPtr> DCH::calculateDescriptor(const pcl::PointCloud<pcl::PointNormal>::Ptr &cloud_,
		const DescriptorParamsPtr &params_,
		const int targetPointIndex_)
{
	pcl::PointNormal target = cloud_->at(targetPointIndex_);
	return calculateDescriptor(cloud_, params_, target);
}

std::vector<BandPtr> DCH::calculateDescriptor(const pcl::PointCloud<pcl::PointNormal>::Ptr &cloud_,
		const DescriptorParamsPtr &params_,
		const pcl::PointNormal &target_)
{
	DCHParams *params = dynamic_cast<DCHParams *>(params_.get());

	// Get target point and surface patch
	pcl::PointCloud<pcl::PointNormal>::Ptr patch = Extractor::getNeighbors(cloud_, target_, params->searchRadius);

	// Extract bands
	std::vector<BandPtr> bands = Extractor::getBands(patch, target_, params);
	DCH::fillSequences(bands, params_);

	return bands;
}

void DCH::computeDense(const pcl::PointCloud<pcl::PointNormal>::Ptr &cloud_,
					   const DescriptorParamsPtr &params_,
					   cv::Mat &descriptors_)
{
	DCHParams *params = dynamic_cast<DCHParams *>(params_.get());
	int sequenceSize = params->getSequenceLength();

	// Resize the matrix in case it doesn't match the required dimensions
	int rows = cloud_->size();
	int cols = sequenceSize * params->bandNumber;
	if (descriptors_.rows != rows || descriptors_.cols != cols)
		descriptors_ = cv::Mat::zeros(rows, cols, CV_32FC1);

	// Extract the descriptors
	for (size_t i = 0; i < cloud_->size(); i++)
	{
		std::vector<BandPtr> bands = DCH::calculateDescriptor(cloud_, params_, cloud_->points[i]);

		for (size_t j = 0; j < bands.size(); j++)
			memcpy(&descriptors_.at<float>(i, j * sequenceSize), &bands[j]->sequenceVector[0], sizeof(float) * sequenceSize);
	}
}

void DCH::computePoint(const pcl::PointCloud<pcl::PointNormal>::Ptr &cloud_,
					   const DescriptorParamsPtr &params_,
					   const int target_,
					   Eigen::VectorXf &descriptor_)
{
	DCHParams *params = dynamic_cast<DCHParams *>(params_.get());
	int sequenceSize = params->getSequenceLength();

	std::vector<BandPtr> bands = DCH::calculateDescriptor(cloud_, params_, target_);
	descriptor_.resize(sequenceSize * bands.size(), 1);

	for (size_t j = 0; j < bands.size(); j++)
		for (size_t k = 0; k < bands[j]->sequenceVector.size(); k++)
			descriptor_(j * sequenceSize + k) = bands[j]->sequenceVector[k];
}

std::vector<Hist> DCH::generateAngleHistograms(const std::vector<BandPtr> &descriptor_,
		const bool useProjection_)
{
	// TODO move this method to the output class, since this is only to generate the histogram generated as output

	std::vector<Hist> histograms = std::vector<Hist>();
	histograms.reserve(descriptor_.size());

	Eigen::Vector3f targetNormal = descriptor_[0]->point.getNormalVector3fMap();
	for (size_t i = 0; i < descriptor_.size(); i++)
	{
		BandPtr band = descriptor_[i];
		histograms.push_back(Hist(ANGLE));

		for (size_t j = 0; j < band->data->size(); j++)
		{
			Eigen::Vector3f normal = band->data->points[j].getNormalVector3fMap();
			histograms.back().add(calculateAngle(targetNormal, normal, band->plane, useProjection_));
		}
	}

	return histograms;
}

void DCH::fillSequences(std::vector<BandPtr> &descriptor_,
						const DescriptorParamsPtr &params_)
{
	DCHParams *params = dynamic_cast<DCHParams *>(params_.get());
	double binSize = params->sequenceBin;
	int binsNumber = params->getSequenceLength();


	for (size_t i = 0; i < descriptor_.size(); i++)
	{
		BandPtr band = descriptor_[i];

		Eigen::Vector3f pointNormal = band->point.getNormalVector3fMap();
		Eigen::Vector3f planeNormal = band->plane.normal();
		Eigen::Vector3f n = planeNormal.cross(pointNormal).normalized();
		Eigen::Hyperplane<float, 3> plane = Eigen::Hyperplane<float, 3>(n, band->point.getVector3fMap());

		// Accumulate
		std::map<int, accumulator_set<double, features<tag::mean, tag::median, tag::min> > > dataMap;
		for (size_t j = 0; j < band->data->size(); j++)
		{
			pcl::PointNormal p = band->data->at(j);
			double theta = calculateAngle(pointNormal, (Eigen::Vector3f) p.getNormalVector3fMap(), band->plane, params->useProjection);
			int index = plane.signedDistance((Eigen::Vector3f) p.getVector3fMap()) / binSize;

			if (dataMap.find(index) == dataMap.end())
				dataMap[index] = accumulator_set<double, features<tag::mean, tag::median, tag::min> >();

			dataMap[index](theta);
		}

		// band->sequenceString = "";
		for (int j = 0; j < binsNumber; j++)
		{
			if (dataMap.find(j) != dataMap.end())
			{
				float value = params->sequenceStat == STAT_MEAN ? (float) mean(dataMap[j]) : (float) median(dataMap[j]);
				band->sequenceVector.push_back(value);
			}
			else
				band->sequenceVector.push_back(5);
		}
	}
}
