#pragma once

#include "../raytracer/common_libs.h"
#include "../raytracer/logs.h"
#include "object.h"
#include "light.h"
#include "scene_utils.h"

class Scene {
public:

    Scene() = default;

    const std::vector<std::unique_ptr<Object>>& GetObjects() const {
        return Objects_;
    }

    std::vector<std::unique_ptr<Object>>& GetNonConstObjects() {
        return Objects_;
    }

    const std::vector<Light>& GetLights() const {
        return Lights_;
    }

    const std::map<std::string, Material>& GetMaterials() const {
        return Materials_;
    }

    const std::vector<Vector> GetVertices() const {
        return Vertices_;
    }

    const std::vector<Vector> GetVectorNormals() const {
        return VectorNormals_;
    }

    const std::vector<Vector> GetVectorTextures() const {
        return VectorTextures_;
    }

    const std::string GetOutputName() const {
        return OutputName_;
    }

    void SetMaterials(const std::map<std::string, Material>& materials) {
        Materials_ = materials;
    }

    void SetOutputName(const std::string& outputname) {
        OutputName_ = outputname;
        size_t pos = OutputName_.find('.');
        if (pos != std::string::npos) {
            OutputName_.replace(pos, std::string::npos, ".png");
        }
    }

    void AddFaceObject(const FaceObject& face) {
        IncrementObjects();
        Objects_.push_back(std::make_unique<FaceObject>(face));
    }

    void AddSphereObject(const SphereObject& sphere) {
        IncrementObjects();
        Objects_.push_back(std::make_unique<SphereObject>(sphere));
    }

    void AddLight(const Light& bulb) {
        Lights_.push_back(bulb);
    }

    void AddMaterial(const std::string& name , const Material& material) {
        Materials_.insert({name, material});
    }

    void AddVertex(const Vector& vec) {
        Vertices_.push_back(vec);
    }

    void AddVectorNormal(const Vector& vec) {
        VectorNormals_.push_back(vec);
    }

    void AddVectorTextures(const Vector& vec) {
        VectorTextures_.push_back(vec);
    }


private:
    std::vector<Vector> Vertices_;
    std::vector<Vector> VectorNormals_;
    std::vector<Vector> VectorTextures_;
    std::vector<std::unique_ptr<Object>> Objects_;
    std::vector<Light> Lights_;
    std::map<std::string, Material> Materials_;
    std::string OutputName_;
};

std::array<Vector, 3> FillVertex(Scene* scene, const std::array<int, 3>& index) {
    std::array<Vector, 3> vertex;

    if (static_cast<size_t>(index[0]) < scene->GetVertices().size()) {
        vertex.at(0) = scene->GetVertices().at(index[0]);
    }
    if (static_cast<size_t>(index[1]) < scene->GetVectorTextures().size()) {
        vertex.at(1) = scene->GetVectorTextures().at(index[1]);
    }
    if (static_cast<size_t>(index[2]) < scene->GetVectorNormals().size()) {
        vertex.at(2) = scene->GetVectorNormals().at(index[2]);
    }
    return vertex;
}

void RetreiveFaces(const std::vector<std::string>& splitted, const std::string& material, Scene* scene, int begin) {
    std::array<Vector, 3> initial;
    std::array<Vector, 3> second;
    std::array<Vector, 3> third;
    std::array<int, 3> index;
    size_t splt_indx = begin;
    int retreived_vrtx = 0;

    while (splt_indx < splitted.size()) {
        splt_indx = FindNonEmpty(splitted, splt_indx);
        if (splt_indx == 0) {
            return;
        }
        index = GetIndex(splitted[splt_indx], {scene->GetVertices().size(), scene->GetVectorTextures().size(), scene->GetVectorNormals().size()});
        if (retreived_vrtx == 2) {
            third = FillVertex(scene, index);
            ++retreived_vrtx;
        } else if (retreived_vrtx == 1) {
            second = FillVertex(scene, index);
            ++retreived_vrtx;
        } else if (retreived_vrtx == 0) {
            initial = FillVertex(scene, index);
            ++retreived_vrtx;
        } 
        if (retreived_vrtx == 3) {
            scene->AddFaceObject({scene->GetMaterials().at(material), initial, second, third});
            second = third;
            retreived_vrtx = 2;
        }
    }
}

inline std::map<std::string, Material> ReadMaterials(const std::string& file_name) {

    std::ifstream mtl_file(file_name);

    if (!mtl_file.is_open()) {
        throw std::runtime_error("Error opening the file :: read material");
    }
    std::string line;
    std::vector<std::string> splitted;
    std::map<std::string, Material> materials;
    bool new_mtl = false;
    Material current_material;
    size_t ind = 0;

    while(std::getline(mtl_file, line)) {
        ChangeToSpace(&line);
        splitted = Split(line, ' ');
        if (splitted.empty()) {
            continue;
        }
        splitted[0].empty() ? ind = FindNonEmpty(splitted, 0) : ind = 0;
        if (splitted[ind] == "newmtl") {
            if (new_mtl) { 
                materials.insert({current_material.name, current_material});
                current_material = Material();
            }
            new_mtl = true;
            current_material.name = splitted[FindNonEmpty(splitted, ind)];
        } else {
            FillMaterial(splitted, &current_material, &ind);
        }
    }
    if (new_mtl) {
        materials.insert({current_material.name, current_material});
    }
    mtl_file.close();
    return materials;
}

std::string GetPath(std::string path, Scene* scene) {
    std::vector<std::string> splitted = Split(path, '/');
    std::string retreived_path = "/";

    for (size_t ind = 0; ind < splitted.size() - 1; ++ind) {
        retreived_path += splitted[ind] + "/";
    }
    scene->SetOutputName(splitted[splitted.size() - 1]);
    return retreived_path;
}

inline Scene ReadScene(const std::string& file_name) { 

    std::ifstream obj_file(file_name);
    std::vector<std::string> splitted;

    if (!obj_file.is_open()) {
        throw std::runtime_error("Error opening the file :: read scene");
    }
    Scene scene;
    std::string path = GetPath(file_name, &scene);
    std::string line;
    std::string current_material;
    size_t ind = 0;

    while(std::getline(obj_file, line)) {
        ChangeToSpace(&line);
        splitted = Split(line, ' ');
        if (splitted.empty()) {
            continue;
        }
        splitted[0].empty() ? ind = FindNonEmpty(splitted, 0) : ind = 0;
        if (splitted[ind] == "v") {
            scene.AddVertex(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "vf") {
            scene.AddVectorTextures(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "vn") {
            scene.AddVectorNormal(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "S") {
            scene.AddSphereObject({scene.GetMaterials().at(current_material), RetreiveSphere(splitted, &ind)});
        } else if (splitted[ind] == "P") {
            scene.AddLight(RetreiveLight(splitted, &ind));
        } else if (splitted[ind] == "f") {
            RetreiveFaces(splitted, current_material, &scene, ind);
        } else if (splitted[ind] == "usemtl") {
                current_material = splitted[FindNonEmpty(splitted, ind)];
        } else if (splitted[ind] == "mtllib") {
            scene.SetMaterials(ReadMaterials(path + splitted[FindNonEmpty(splitted, ind)]));
        } 
    }
    obj_file.close();

    return scene;
}

void PrintScene (const Scene& scene) {
    for (auto [key, material] : scene.GetMaterials()) {
        std::cout << "Material " << key << std::endl;
        std::cout << "Ka: ";
        PrintVec(material.ambient_color);
        std::cout << "Kd: ";
        PrintVec(material.diffuse_color);
        std::cout << "Ks: ";
        PrintVec(material.specular_color);
        std::cout << "Ke: ";
        PrintVec(material.intensity);
        std::cout << "Ns: " << material.specular_exponent << std::endl;
        std::cout << "Ni: " << material.refraction_index << std::endl;
        std::cout << "al: ";
        PrintVec(material.albedo);
        std::cout << "\n";
    }
    std::cout << "\n\t  \n\n";
    for (const auto& element : scene.GetObjects()) {
        element->PrintPrivateMembers();
    }

    std::cout << "\n\t Lights \n\n";
    for (auto light : scene.GetLights()) {
        std::cout << "Added a light with position: ";
        PrintVec(light.position);
        std::cout << "   \t\tand intensivity ";
        PrintVec(light.intensity);
    }
}