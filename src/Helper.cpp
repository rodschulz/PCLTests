/**
 * Author: rodrigo
 * 2015
 */
#include "Helper.h"
#include <pcl/filters/filter.h>
#include <ctype.h>

Helper::Helper()
{
}

Helper::~Helper()
{
}

void Helper::removeNANs(PointCloud<PointXYZ>::Ptr &_cloud)
{
	std::vector<int> mapping;
	removeNaNFromPointCloud(*_cloud, *_cloud, mapping);
}

void Helper::createColorCloud(const PointCloud<PointXYZ>::Ptr &_cloud, PointCloud<PointXYZRGB>::Ptr &_coloredCloud, const uint8_t _r, const uint8_t _g, const uint8_t _b)
{
	_coloredCloud->clear();
	_coloredCloud->resize(_cloud->size());
	_coloredCloud->width = _cloud->size();
	_coloredCloud->height = 1;

	uint32_t color = ((uint32_t) _r << 16 | (uint32_t) _g << 8 | (uint32_t) _b);
	for (int i = 0; i < _cloud->width; i++)
	{
		_coloredCloud->points[i].x = _cloud->points[i].x;
		_coloredCloud->points[i].y = _cloud->points[i].y;
		_coloredCloud->points[i].z = _cloud->points[i].z;
		_coloredCloud->points[i].rgb = *reinterpret_cast<float*>(&color);
	}
}

float Helper::getColor(const uint8_t _r, const uint8_t _g, const uint8_t _b)
{
	uint32_t color = ((uint32_t) _r << 16 | (uint32_t) _g << 8 | (uint32_t) _b);
	float finalColor = *reinterpret_cast<float*>(&color);
	return finalColor;
}

template<typename T>
class data_filler
{
public:
	data_filler()
	{
	}
	T operator()()
	{
		return rand() / (T) RAND_MAX;
	}
};

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/density.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <vector>
#include <algorithm>
using namespace boost;
using namespace boost::accumulators;
void Helper::calculateMeanCurvature(vector<Band> &_bands, const PointXYZ &_point, vector<double> &_curvatures)
{
	//accumulator_set<double, stats<tag::density> > acc(tag::density::cache_size = 10, tag::density::num_bins = 5);
	//iterator_range<std::vector<std::pair<double, double> >::iterator> hist = density(acc);
	//for (int k = 0; k < hist.size(); k++)
	//	cout << "first: " << hist[k].first << " - second: " << hist[k].second << "\n";

	//////

	_curvatures.reserve(_bands.size());
	Vector3f targetPoint = _point.getVector3fMap();

	for (size_t i = 0; i < _bands.size(); i++)
	{
		Vector3f targetNormal = _bands[i].pointNormal;

		if (_bands[i].radialBand)
		{
			double curvature = 0;
			for (size_t j = 0; j < _bands[i].normalBand->size(); j++)
			{
				Vector3f point = _bands[i].dataBand->points[j].getVector3fMap();
				Vector3f normal = _bands[i].normalBand->points[j].getNormalVector3fMap();

				// Create a plane containing the target point, its normal and the current point
				Vector3f targetToPoint = point - targetPoint;
				Vector3f planeNormal = targetNormal.cross(targetToPoint);
				planeNormal.normalize();
				Hyperplane<float, 3> plane = Hyperplane<float, 3>(planeNormal, targetPoint);

				// Project current point's normal onto the plane and calculate the point's curvature
				Vector3f projectedNormal = plane.projection(normal);

				Vector3f pointDiff = targetPoint - point;
				double pointDistance = pointDiff.norm();
				Vector3f normalDiff = targetNormal - projectedNormal;
				double normalDistance = normalDiff.norm();

				if (pointDistance > 0)
					curvature += (normalDistance / pointDistance);
			}
			curvature /= _bands[i].normalBand->size();
			_curvatures.push_back(curvature);

		}
		else
		{
			double curvature = 0;
			for (size_t j = 0; j < _bands[i].normalBand->size(); j++)
			{
				Vector3f point = _bands[i].dataBand->points[j].getVector3fMap();
				Vector3f normal = _bands[i].normalBand->points[j].getNormalVector3fMap();

				Vector3f projectedPoint = _bands[i].planeAlong.projection(point);
				Vector3f projectedNormal = _bands[i].planeAlong.projection(normal);

				Vector3f pointDiff = targetPoint - projectedPoint;
				double pointDistance = pointDiff.norm();
				Vector3f normalDiff = targetNormal - projectedNormal;
				double normalDistance = normalDiff.norm();

				if (pointDistance > 0)
					curvature += (normalDistance / pointDistance);
			}

			curvature /= _bands[i].normalBand->size();
			_curvatures.push_back(curvature);
		}
	}
}

bool Helper::isNumber(const string &_str)
{
	bool number = true;
	for (size_t i = 0; i < _str.size(); i++)
	{
		number = number && isdigit(_str[i]);
	}
	return number;
}
