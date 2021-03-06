/*
    Collisions - a real-time simulation program of colliding particles.
    Copyright (C) 2011 - 2014  G. Endignoux

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/gpl-3.0.txt
*/

#include "edit_populations.hpp"

#include <QHeaderView>

EditPopulations::EditPopulations(EditPolygone* editPolygone) :
    EditWithPolygone(editPolygone),
    mTableView(new QTableView),
    mSpinboxDelegate(new SpinBoxDelegate(5000)),
    mNotnullDelegate(new DoubleDelegate(1e-10)),
    mPositiveDelegate(new DoubleDelegate(0)),
    mColorDelegate(new ColorDelegate)
{
    mModel->setColumnCount(5);
    mModel->setHorizontalHeaderLabels(QStringList() << "nombre" << "rayon" << "masse" << "vitesse" << "couleur");

    mTableView->setModel(mModel);
    mTableView->setItemDelegateForColumn(0, mSpinboxDelegate);
    mTableView->setItemDelegateForColumn(1, mNotnullDelegate);
    mTableView->setItemDelegateForColumn(2, mNotnullDelegate);
    mTableView->setItemDelegateForColumn(3, mPositiveDelegate);
    mTableView->setItemDelegateForColumn(4, mColorDelegate);
    mTableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    this->addViewWidget(mTableView);

    this->connectSelection(true);
    mIndex = -1;
}


void EditPopulations::setPopulations(QList<ConfigPopulation> populations)
{
    this->connectSelection(false);

    mModel->removeRows(0, mModel->rowCount());
    mPolygones.clear();
    mIndex = 0;

    for (int i = 0 ; i < populations.size() ; ++i)
    {
        this->addPolygone();
        mPolygones[i] = populations[i].polygone();

        QString name = ColorEditor::strFromColor(populations[i].color());
        mModel->appendRow(QList<QStandardItem*>() << new QStandardItem(QString::number(populations[i].taille())) << new QStandardItem(QString::number(populations[i].rayon())) << new QStandardItem(QString::number(populations[i].masse())) << new QStandardItem(QString::number(populations[i].vitesse())) << new QStandardItem(name));
        mModel->setData(mModel->index(mIndex, 4, QModelIndex()), populations[i].color(), Qt::DecorationRole);
        mModel->setData(mModel->index(mIndex, 4, QModelIndex()), populations[i].color(), Qt::UserRole);
    }

    mTableView->selectRow(mIndex);
    this->connectSelection(true);
}

QList<ConfigPopulation> EditPopulations::config() const
{
    QList<ConfigPopulation> result;

    for (int i = 0 ; i < mPolygones.size() ; ++i)
    {
        unsigned int nombre = mModel->data(mModel->index(i, 0, QModelIndex()), Qt::EditRole).toUInt();
        double rayon = mModel->data(mModel->index(i, 1, QModelIndex()), Qt::EditRole).toDouble();
        double masse = mModel->data(mModel->index(i, 2, QModelIndex()), Qt::EditRole).toDouble();
        double vitesse = mModel->data(mModel->index(i, 3, QModelIndex()), Qt::EditRole).toDouble();
        QColor couleur = QColor(mModel->data(mModel->index(i, 4, QModelIndex()), Qt::DecorationRole).toString());
        result.append(ConfigPopulation(nombre, rayon, masse, couleur, mPolygones[i], vitesse));
    }

    return result;
}


void EditPopulations::selectPolygone()
{
    QModelIndexList selection = mTableView->selectionModel()->selectedIndexes();
    if (selection.isEmpty())
        mIndex = -1;
    else
        mIndex = selection[0].row();
    this->setPolygone();
}


void EditPopulations::connectSelection(bool connect)
{
    if (connect)
        QObject::connect(mTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectPolygone()));
    else
        QObject::disconnect(mTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectPolygone()));
}

void EditPopulations::add()
{
    this->addPolygone();
    this->connectSelection(false);

    mModel->appendRow(QList<QStandardItem*>() << new QStandardItem(QString::number(10)) << new QStandardItem(QString::number(1)) << new QStandardItem(QString::number(1)) << new QStandardItem(QString::number(1)) << new QStandardItem("black"));
    mModel->setData(mModel->index(mIndex, 4, QModelIndex()), Qt::black, Qt::DecorationRole);
    mModel->setData(mModel->index(mIndex, 4, QModelIndex()), QColor(Qt::black).name(), Qt::UserRole);

    mTableView->selectRow(mIndex);
    this->setPolygone();

    this->connectSelection(true);
}

void EditPopulations::remove()
{
    if (mIndex == -1)
        return;

    this->connectSelection(false);

    this->removePolygone();
    mTableView->selectionModel()->select(mModel->index(mIndex, 0, QModelIndex()), QItemSelectionModel::SelectCurrent);
    this->setPolygone();

    this->connectSelection(true);
}
