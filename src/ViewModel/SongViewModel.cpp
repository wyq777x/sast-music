#include "SongViewModel.h"
#include <Service/NeteaseCloudMusic/CloudMusicClient.h>
using namespace NeteaseCloudMusic;

SongViewModel::SongViewModel(QObject* parent) : QAbstractListModel(parent) {}

int SongViewModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return model.count();
}

QVariant SongViewModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    auto element = model[index.row()];
    switch (role) {
    case Role::SongId:
        return (int)element.id;
    case Role::Name:
        return element.name;
    case Role::Alias:
        return element.alias;
    case Role::Artists:
        return element.artists;
    case Role::Album:
        return element.album;
    case Role::ImgUrl:
        return element.imgUrl;
    }
    return QVariant();
}

QHash<int, QByteArray> SongViewModel::roleNames() const {
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[Role::Name] = "name";
        roles[Role::SongId] = "songId";
        roles[Role::Alias] = "alias";
        roles[Role::Artists] = "artists";
        roles[Role::Album] = "album";
        roles[Role::ImgUrl] = "imgUrl";
    }
    return roles;
}

void SongViewModel::loadSongs(PlaylistId playListId) {
    CloudMusicClient::getInstance()->getPlaylistDetail(playListId, [this](Result<PlaylistDetailEntity> result) {
        if (result.isErr()) {
            emit loadSongsFailed(result.unwrapErr().message);
            return;
        }
        auto songs = result.unwrap().tracks;
        emit beginResetModel();
        for (int i = 0; i < 12; ++i) {
            model.emplace_back(songs[i]);
        }
        emit endResetModel();
        setCount(songs.count());
        emit loadSongsSuccess();
    });
}

int SongViewModel::getCount() const {
    return count;
}

void SongViewModel::setCount(int newCount) {
    if (count == newCount)
        return;
    count = newCount;
    emit countChanged();
}