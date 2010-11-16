/*=======================================================
 Copyright (c) avp::ptr, 2010
=======================================================*/

#ifndef _GPSVIEW_H_
#define _GPSVIEW_H_


#include "Drawable.h"
#include "GpsData.h"
#include "Walk.h"


class GpsView : public Drawable
{
public:

	/**
	 * \brief Default constructor. 
	**/
	GpsView(void);

	/**
	 * \brief Destructor. 
	**/
	virtual ~GpsView(void);

	/**
	 * \brief Returns a GpsData object. 
	 * \return A GpsData pointer. 
	**/
	const GpsData& getGpsData() const { return *m_gpsData; }

	/**
	 * \brief Sets the GpsData.
	 * \param gpsData A GpsData pointer.
	**/
	void setGpsData(GpsData* gpsData);
	void setWalk( Walk* walk);

protected:
	GpsData* m_gpsData;		///< Container for gps data.
	Walk* m_walk;

};


#endif // _GPSVIEW_H_