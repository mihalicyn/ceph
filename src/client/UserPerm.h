// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2016 Red Hat
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#ifndef CEPH_CLIENT_USERPERM_H
#define CEPH_CLIENT_USERPERM_H

struct UserPerm
{
private:
  uid_t m_uid;
  gid_t m_gid;
  uid_t m_inode_owner_uid;
  gid_t m_inode_owner_gid;
  int gid_count;
  gid_t *gids;
  bool alloced_gids;
  void deep_copy_from(const UserPerm& b) {
    if (alloced_gids) {
      delete[] gids;
      alloced_gids = false;
    }
    m_uid = b.m_uid;
    m_gid = b.m_gid;
    m_inode_owner_uid = b.m_inode_owner_uid;
    m_inode_owner_gid = b.m_inode_owner_gid;
    gid_count = b.gid_count;
    if (gid_count > 0) {
      gids = new gid_t[gid_count];
      alloced_gids = true;
      for (int i = 0; i < gid_count; ++i) {
	gids[i] = b.gids[i];
      }
    }
  }
public:
  UserPerm() : m_uid(-1), m_gid(-1), m_inode_owner_uid(-1), m_inode_owner_gid(-1), gid_count(0),
	       gids(NULL), alloced_gids(false) {}
  UserPerm(uid_t uid, gid_t gid, int ngids=0, gid_t *gidlist=NULL, uid_t inode_owner_uid=-1, gid_t inode_owner_gid=-1) :
	    m_uid(uid), m_gid(gid),
      m_inode_owner_uid(inode_owner_uid != (uid_t)-1 ? inode_owner_uid : uid),
      m_inode_owner_gid(inode_owner_gid != (gid_t)-1 ? inode_owner_gid : gid),
      gid_count(ngids), gids(gidlist), alloced_gids(false) {}
  UserPerm(const UserPerm& o) : UserPerm() {
    deep_copy_from(o);
  }
  UserPerm(UserPerm && o) {
    m_uid = o.m_uid;
    m_gid = o.m_gid;
    m_inode_owner_uid = o.m_inode_owner_uid;
    m_inode_owner_gid = o.m_inode_owner_gid;
    gid_count = o.gid_count;
    gids = o.gids;
    alloced_gids = o.alloced_gids;
    o.gids = NULL;
    o.gid_count = 0;
  }
  ~UserPerm() {
    if (alloced_gids)
      delete[] gids;
  }
  UserPerm& operator=(const UserPerm& o) {
    deep_copy_from(o);
    return *this;
  }

  uid_t uid() const { return m_uid != (uid_t)-1 ? m_uid : ::geteuid(); }
  gid_t gid() const { return m_gid != (gid_t)-1 ? m_gid : ::getegid(); }
  uid_t inode_owner_uid() const { return m_inode_owner_uid != (uid_t)-1 ? m_inode_owner_uid : uid(); }
  gid_t inode_owner_gid() const { return m_inode_owner_gid != (gid_t)-1 ? m_inode_owner_gid : gid(); }
  bool gid_in_groups(gid_t id) const {
    if (id == gid()) return true;
    for (int i = 0; i < gid_count; ++i) {
      if (id == gids[i]) return true;
    }
    return false;
  }
  int get_gids(const gid_t **_gids) const { *_gids = gids; return gid_count; }
  void init_gids(gid_t* _gids, int count) {
    gids = _gids;
    gid_count = count;
    alloced_gids = true;
  }
  void shallow_copy(const UserPerm& o) {
    m_uid = o.m_uid;
    m_gid = o.m_gid;
    m_inode_owner_uid = o.m_inode_owner_uid;
    m_inode_owner_gid = o.m_inode_owner_gid;
    gid_count = o.gid_count;
    gids = o.gids;
    alloced_gids = false;
  }
};

#endif
