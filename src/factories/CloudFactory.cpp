/**
 * Author: rodrigo
 * 2015
 */
#include "CloudFactory.h"
#include "PointFactory.h"

CloudFactory::CloudFactory()
{
}

CloudFactory::~CloudFactory()
{
}

void CloudFactory::generateCube(const double _size, const pcl::PointXYZ &_center, pcl::PointCloud<pcl::PointXYZ>::Ptr &_cloud)
{
	_cloud->clear();

	double minX = _center.x - _size * 0.5;
	double minY = _center.y - _size * 0.5;
	double minZ = _center.z - _size * 0.5;

	double maxX = _center.x + _size * 0.5;
	double maxY = _center.y + _size * 0.5;
	double maxZ = _center.z + _size * 0.5;

	double step = _size * 0.01;

	// Generate faces fixed in X axis
	for (double y = minY; y <= maxY; y += step)
	{
		for (double z = minZ; z <= maxZ; z += step)
		{
			_cloud->push_back(PointFactory::makePointXYZ(minX, y, z));
			_cloud->push_back(PointFactory::makePointXYZ(maxX, y, z));
		}
	}

	// Generate faces fixed in Y axis
	for (double x = minX; x <= maxX; x += step)
	{
		for (double z = minZ; z <= maxZ; z += step)
		{
			_cloud->push_back(PointFactory::makePointXYZ(x, minY, z));
			_cloud->push_back(PointFactory::makePointXYZ(x, maxY, z));
		}
	}

	// Generate faces fixed in Z axis
	for (double x = minX; x <= maxX; x += step)
	{
		for (double y = minY; y <= maxY; y += step)
		{
			_cloud->push_back(PointFactory::makePointXYZ(x, y, minZ));
			_cloud->push_back(PointFactory::makePointXYZ(x, y, maxZ));
		}
	}
}

void CloudFactory::generateCylinder(const double _radius, const double _height, const pcl::PointXYZ &_center, pcl::PointCloud<pcl::PointXYZ>::Ptr &_cloud)
{
	_cloud->clear();

	double minZ = _center.z - _height * 0.5;
	double maxZ = _center.z + _height * 0.5;
	double stepZ = _height * 0.01;

	double angularStep = 2 * M_PI * 0.005;
	double radialStep = _radius * 0.02;

	// Generate cylinder top and bottom
	for (double angle = 0; angle < 2 * M_PI; angle += angularStep)
	{
		for (double r = _radius; r > 0; r -= radialStep)
		{
			_cloud->push_back(PointFactory::makePointXYZ(r * cos(angle) + _center.x, r * sin(angle) + _center.y, minZ));
			_cloud->push_back(PointFactory::makePointXYZ(r * cos(angle) + _center.x, r * sin(angle) + _center.y, maxZ));
		}
	}

	// Generate cylinder side
	for (double z = minZ; z <= maxZ; z += stepZ)
	{
		for (double angle = 0; angle < 2 * M_PI; angle += angularStep)
		{
			_cloud->push_back(PointFactory::makePointXYZ(_radius * cos(angle) + _center.x, _radius * sin(angle) + _center.y, z));
		}
	}
}

void CloudFactory::generateSphere(const double _radius, const pcl::PointXYZ &_center, pcl::PointCloud<pcl::PointXYZ>::Ptr &_cloud)
{
	_cloud->clear();

	double angularStep = 2 * M_PI * 0.005;
	for (double theta = 0; theta < 2 * M_PI; theta += angularStep)
	{
		for (double phi = 0; phi < 2 * M_PI; phi += angularStep)
		{
			_cloud->push_back(PointFactory::makePointXYZ(_radius * sin(theta) * cos(phi) + _center.x, _radius * sin(theta) * sin(phi) + _center.y, _radius * cos(theta) + _center.z));
		}
	}
}

pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr CloudFactory::createColorCloud(const pcl::PointCloud<pcl::PointNormal>::Ptr &_cloud, uint32_t _color)
{
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr coloredCloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>());
	coloredCloud->reserve(_cloud->size());

	float color = *reinterpret_cast<float*>(&_color);
	for (unsigned int i = 0; i < _cloud->width; i++)
	{
		pcl::PointNormal p = _cloud->points[i];
		coloredCloud->push_back(PointFactory::makePointXYZRGBNormal(p.x, p.y, p.z, p.normal_x, p.normal_y, p.normal_z, p.curvature, color));
	}

	return coloredCloud;
}

pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr CloudFactory::createColorCloud(const pcl::PointCloud<pcl::PointNormal>::Ptr &_cloud, uint8_t _r, uint8_t _g, uint8_t _b)
{
	pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr coloredCloud(new pcl::PointCloud<pcl::PointXYZRGBNormal>());
	coloredCloud->reserve(_cloud->size());

	for (unsigned int i = 0; i < _cloud->width; i++)
	{
		pcl::PointNormal p = _cloud->points[i];
		coloredCloud->push_back(PointFactory::makePointXYZRGBNormal(p.x, p.y, p.z, p.normal_x, p.normal_y, p.normal_z, p.curvature, _r, _g, _b));
	}

	return coloredCloud;
}
