#include "bucket.h"
#include "buffer_pool.h"
#include "data_placement_engine.h"

#include <iostream>

namespace hermes {

namespace api {

Status Bucket::Contain_blob(const std::string& blob_name) {
	Status ret = 0;
	
	LOG(INFO) << "Checking if blob " << blob_name << " exists in Bucket "
	          << name_ << '\n';
	
	if (blobs_.find(blob_name) != blobs_.end())
		ret = 1;
			
  return ret;
}

Status Bucket::Rename(const std::string &new_name, Context &ctx) {
  (void)ctx;
  Status ret = 0;
    
  LOG(INFO) << "Renaming a bucket to" << new_name << '\n';
    
  return ret;
}


Status Bucket::Release(Context &ctx) {
  (void)ctx;
  Status ret = 0;

  LOG(INFO) << "Releasing bucket " << '\n';

  for (const auto &name_buffer_ids : blobs_) {
    ReleaseBuffers(&hermes_->context_, blobs_[name_buffer_ids.first]);
  }
  blobs_.clear();

  return ret;
}

Status Bucket::Put(const std::string &name, const Blob &data, Context &ctx) {
  (void)ctx;
  Status ret = 0;

  LOG(INFO) << "Attaching blob " << name << "to Bucket " << '\n';

  TieredSchema schema = CalculatePlacement(data.size(), ctx);
  while (schema.size() == 0) {
    // NOTE(chogan): Keep running the DPE until we get a valid placement
    schema = CalculatePlacement(data.size(), ctx);
  }

  std::vector<BufferID> buffer_ids = GetBuffers(&hermes_->context_, schema);
  while (buffer_ids.size() == 0) {
    // NOTE(chogan): This loop represents waiting for the BufferOrganizer to
    // free some buffers if it needs to. It will probably be handled through the
    // messaging service.
    buffer_ids = GetBuffers(&hermes_->context_, schema);
  }

  hermes::Blob blob = {};
  blob.data = (u8 *)data.data();
  blob.size = data.size();
  WriteBlobToBuffers(&hermes_->context_, blob, buffer_ids);

  // TODO(chogan): UpdateMetadata();
  blobs_[name] = buffer_ids;

  return ret;
}

size_t Bucket::Get(const std::string &name, Blob& user_blob, Context &ctx) {
  (void)ctx;

  size_t ret = 0;
  LOG(INFO) << "Getting Blob " << name << " from bucket " << name_ << '\n';

  if (user_blob.size() == 0) {
    ret = GetBlobSize(&hermes_->context_, blobs_[name]);
  } else {
    hermes::Blob blob = {};
    blob.data = user_blob.data();
    blob.size = user_blob.size();

    ret = ReadBlobFromBuffers(&hermes_->context_, &blob, blobs_[name]);
  }

  return ret;
}

Status Bucket::DeleteBlob(const std::string &name, Context &ctx) {
  (void)ctx;
  Status ret = 0;

  LOG(INFO) << "Deleting Blob " << name << " from bucket " << name_ << '\n';

  ReleaseBuffers(&hermes_->context_, blobs_[name]);
  blobs_.erase(name);

  return ret;
}

Status Bucket::RenameBlob(const std::string &old_name,
                          const std::string &new_name,
                          Context &ctx) {
  (void)ctx;
  Status ret = 0;
    
  LOG(INFO) << "Renaming Blob " << old_name << " to " << new_name << '\n';
    
  return ret;
}
    
} // api namespace
} // hermes namespace