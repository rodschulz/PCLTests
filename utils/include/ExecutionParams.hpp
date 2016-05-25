/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <string>
#include <vector>

enum ExecutionType
{
	EXECUTION_NONE, EXECUTION_DESCRIPTOR, EXECUTION_CLUSTERING, EXECUTION_METRIC
};

enum SynCloudType
{
	CLOUD_NONE, CLOUD_CUBE, CLOUD_CYLINDER, CLOUD_SPHERE, CLOUD_HALF_SPHERE, CLOUD_PLANE
};

enum SmoothingType
{
	SMOOTHING_NONE, SMOOTHING_GAUSSIAN, SMOOTHING_MLS
};

enum SequenceStat
{
	STAT_NONE, STAT_MEAN, STAT_MEDIAN
};

enum ClusteringImplementation
{
	CLUSTERING_NONE, CLUSTERING_OPENCV, CLUSTERING_CUSTOM, CLUSTERING_STOCHASTIC
};

enum MetricType
{
	METRIC_NONE, METRIC_EUCLIDEAN, METRIC_CLOSEST_PERMUTATION
};

class ExecutionParams
{
public:
	ExecutionParams();
	~ExecutionParams() {};

	// Returns the execution type associated to the given string
	static ExecutionType getExecutionType(const std::string &_type);

	// Returns the synthetic cloud type associated to the given string
	static SynCloudType getSynCloudType(const std::string &_type);

	// Returns the cloud smoothing type associated to the given string
	static SmoothingType getSmoothingType(const std::string &_type);

	// Returns the statistic type associated to the given string
	static SequenceStat getStatType(const std::string &_type);

	// Returns the clustering implementation associated to the given string
	static ClusteringImplementation getClusteringImplementation(const std::string &_type);

	// Returns the metric type associated to the given string
	static MetricType getMetricType(const std::string &_type);

	// Returns a string with the hex representation of the hash calculated for the current params instance
	std::string getHash() const;

	// Returns the angular range of the bands according to the current config
	double getBandsAngularRange() const;

	// Returns the angular step of the bands according to the current config
	double getBandsAngularStep() const;

	// Returns the sequence length of the bands (number of bins) according to the current config
	int getSequenceLength() const;

	ExecutionType executionType;			// Type of execution to run

	std::string inputLocation;			// Location of the input file
	int targetPoint;				// Target point

	double patchSize;				// Search radius used with the SEARCH_RADIUS method
	double normalEstimationRadius;			// Radius used to calculate normals
	int bandNumber;					// Number of bands to sample
	double bandWidth;				// Width of each band
	bool bidirectional;				// Flag indicating if each band has to be analyzed bidirectional or not
	bool useProjection;				// Flag indicating if angle calculation has to be projecting the angles over the band's plane

	double sequenceBin;				// Size of the bins used in the sequence construction
	SequenceStat sequenceStat;			// Statistic to use in the sequence calculation

	bool useSynthetic;				// Flag indicating if a synthetic has to be used
	SynCloudType synCloudType;			// Desired synthetic cloud

	SmoothingType smoothingType;			// Type of smoothing algorithm to use
	double gaussianSigma;				// Sigma used for the gaussian smoothning
	double gaussianRadius;				// Search radius used for the gaussian smoothing
	double mlsRadius;				// Search radius used for the mls smoothing

	bool genElbowCurve;				// Flag indicating if an elbow graph has to be generated
	bool genDistanceMatrix;				// Flag indicating if the distance matrix image has to be generated

	bool labelData;					// Flag indicating if data has to be labeled instead of calculate clusters
	std::string centersLocation;			// Location of the file storing the centers for data labeling

	ClusteringImplementation implementation;	// Implementation of clustering to be used
	MetricType metric;				// Type of metric to use in clustering execution
	int clusters;					// Number of clusters used in the clustering test
	int maxIterations;				// Clustering max iterations
	double stopThreshold;				// Clustering stop threshold
	int attempts;					// Number of attemtps to try when clustering
	std::string cacheLocation;			// Location of the cachefiles
	bool useConfidence;				// Use confidence, if the metric allows it

	MetricType targetMetric;			// Metric type to be used in the metric evaluation
	std::vector<std::string> metricArgs;		// Arguments to initialize the target metric
};