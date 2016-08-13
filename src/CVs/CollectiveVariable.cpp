#include "CollectiveVariable.h"
#include "json/json.h"
#include "schema.h"
#include "../Drivers/DriverException.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "AtomCoordinateCV.h"
#include "AtomPositionCV.h"
#include "TorsionalCV.h"
#include "AtomSeparationCV.h"
#include "AngleCV.h"
#include "RadiusOfGyrationCV.h"
#include "CenterofMassDistanceCV.h"
#include "RMSDCV.h"

using namespace Json;

namespace SSAGES
{
	CollectiveVariable* CollectiveVariable::BuildCV(const Json::Value &json)
	{
		return BuildCV(json, "#/CVs");
	}

	CollectiveVariable* CollectiveVariable::BuildCV(const Json::Value &json,
						const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		CollectiveVariable* cv = nullptr;

		// Random device for seed generation. 
		// std::random_device rd;
		// auto maxi = std::numeric_limits<int>::max();
		// auto seed = json.get("seed", rd() % maxi).asUInt();

		// Get move type. 
		std::string type = json.get("type", "none").asString();

		if(type == "AtomCoordinate")
		{
			reader.parse(JsonSchema::AtomCoordinateCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			auto atomid = json.get("atom id", -1).asInt();
			auto indextype = json.get("dimension","x").asString();

			int index = -1;

			if(indextype == "x")
				index = 0;
			else if(indextype == "y")
				index = 1;
			else if(indextype == "z")
				index = 2;
			else
				throw BuildException({"Could not obtain AtomCoordinate specified."});

			auto* c = new AtomCoordinateCV(atomid, index);

			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "AtomPosition")
		{
			reader.parse(JsonSchema::AtomPositionCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			auto atomid = json.get("atom id", -1).asInt();
			Vector3 position;

			position[0]=json["position"][0].asDouble();
			position[1]=json["position"][1].asDouble();
			position[2]=json["position"][2].asDouble();

			auto fixx = json.get("fixx", false).asBool();
			auto fixy = json.get("fixy", false).asBool();
			auto fixz = json.get("fixz", false).asBool();

			auto* c = new AtomPositionCV(atomid, position, fixx, fixy, fixz);

			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "Torsional")
		{
			reader.parse(JsonSchema::TorsionalCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			std::vector<int> atomids;
			for(auto& s : json["atom ids"])
				atomids.push_back(s.asInt());

			auto periodic = json.get("periodic", true).asBool();

			auto* c = new TorsionalCV(atomids[0], atomids[1], atomids[2], atomids[3], periodic);

			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "AtomSeparation")
		{
			reader.parse(JsonSchema::AtomSeparationCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			auto atomid1 = json.get("atom id 1", -1).asInt();
			auto atomid2 = json.get("atom id 2", -1).asInt();

			auto* c = new AtomSeparationCV(atomid1, atomid2);

			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "Angle")
		{
			reader.parse(JsonSchema::AngleCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			std::vector<int> atomids;
			for(auto& s : json["atom ids"])
				atomids.push_back(s.asInt());

			auto* c = new AngleCV(atomids[0], atomids[1], atomids[2]);

			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "RadiusOfGyration")
		{
			reader.parse(JsonSchema::RadiusOfGyrationCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			std::vector<int> atomids;
			for(auto& s : json["atom ids"])
				atomids.push_back(s.asInt());

			auto* c = new RadiusOfGyrationCV(atomids, json.get("use_range", false).asBool());
			
			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "CenterofMassDistance")
		{
			reader.parse(JsonSchema::CenterofMassDistanceCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			std::vector<int> atomids1;
			for(auto& s : json["atom ids1"])
				atomids1.push_back(s.asInt());

			std::vector<int> atomids2;
			for(auto& s : json["atom ids2"])
				atomids2.push_back(s.asInt());

			auto* c = new CenterofMassDistanceCV(atomids1, atomids2, json.get("use_range1", false).asBool(), json.get("use_range2", false).asBool());
			
			cv = static_cast<CollectiveVariable*>(c);
		}
		else if(type == "RMSD")
		{
			reader.parse(JsonSchema::RMSDCV, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
			
			std::vector<int> atomids;
			for(auto& s : json["atom ids"])
				atomids.push_back(s.asInt());
			auto reference = json.get("reference"," ").asString(); 

			auto* c = new RMSDCV(atomids, reference, json.get("use_range", false).asBool());
			
			cv = static_cast<CollectiveVariable*>(c);
		}
		else
		{
			throw BuildException({path + ": Unknown CV type specified."+type+" is not a valid type!"});
		}

		return cv;
	}

	void CollectiveVariable::BuildCV(const Json::Value &json, 
						  CVList &cvlist,
						  const std::string &path)
	{
		ArrayRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::CVs, schema);
		validator.Parse(schema, path);

		// Validate high level schema.
		validator.Validate(json, path);
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through CVs.
		int i = 0;
		for(auto& m : json)
		{
			cvlist.push_back(BuildCV(m, path + "/" + std::to_string(i)));
			++i;
		}
	}
}
