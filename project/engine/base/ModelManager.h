#pragma once
#include <map>
#include <string>
#include <memory>
class Model;
class ModelCommon;
class DirectXCommon;
class ModelManager {
public:
	
	/// <summary>
	/// シングルトンインスタンス取得
	/// </summary>
	static ModelManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 開放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// モデルの読み込み
	/// </summary>
	/// <param name="filePath">モデルのファイルパス</param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルのファイルパス</param>
	Model* FindModel(const std::string& filePath);

private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

private:

	//インスタンス
	static ModelManager* instance_;

	ModelCommon* modelCommon_ = nullptr;

	//モデルデータコンテナ
	std::map<std::string, std::unique_ptr<Model>> models_;
};

