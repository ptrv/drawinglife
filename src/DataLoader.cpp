#include "DataLoader.h"
#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "DBReader.h"

DataLoader::DataLoader()
{
}

void DataLoader::prepareGpsData(DrawingLifeApp& app)
{
    app.resetData();
}

// -----------------------------------------------------------------------------

void DataLoader::processGpsData(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    Timeline& timeline = app.getTimeline();
    GpsDataVector& gpsDatas = app.getGpsDataVector();
    WalkVector& walks = app.getWalkVector();
    MagicBoxVector& magicBoxes = app.getMagicBoxVector();

    const std::vector<CurrentImageData>& imageList =
            app.getCurrentPointImageList();
    const boost::ptr_vector<ofImage>& images = app.getCurrentPointImages();

    ofLog(OF_LOG_VERBOSE, "------------------------\n");
    if (gpsDatas.size() > 0)
    {
        timeline.setTimeline(gpsDatas);
//        for(unsigned int i = 0; i < m_timeline->getTimeline().size(); ++i)
//        {
//            ofLog(OF_LOG_VERBOSE, "%s : %d : %li", m_timeline->getTimeline()[i].timeString.c_str(),
//                                                    m_timeline->getTimeline()[i].id,
//                                                    (long)(m_timeline->getTimeline()[i].secs));
//        }
        Utils::calculateGlobalMinMaxValues(app);

        Walk::setTrackAlpha(settings.getAlphaDot());

        Walk::setDotSize(settings.getDotSize());

        for(size_t i = 0; i < walks.size(); ++i)
        {
            Walk& walk = walks[i];
//            m_walks[i]->setDotColors();

            if(!settings.isBoundingBoxAuto() && !settings.isMultiMode())
            {
                //m_walks[i]->setMagicBoxStatic(m_magicBoxes[i]);
                double bbLat = settings.getBoundingBoxLat();
                double bbLon = settings.getBoundingBoxLon();
                walk.setMagicBoxStatic(&magicBoxes[i], bbLat, bbLon);
            }
            else
            {
                if(settings.isMultiMode())
                {
                    double bbLat = settings.getBoundingBoxLat();
                    double bbLon = settings.getBoundingBoxLon();
                    walk.setMagicBoxStatic(&app.getMagicBox(), bbLat, bbLon);
//                    walk.setMagicBox(m_magicBox);
                }
                else
                {
                    walk.setMagicBox(&magicBoxes[i]);
                }
            }

            if (app.getIsImageAsCurrentPoint()
                    && images.size() >= settings.getNumPerson())
            {
                walk.setCurrentPointImage(images[i], imageList[i].alpha);
            }
        }
    }
}

// -----------------------------------------------------------------------------

bool DataLoader::loadGpsDataCity(DrawingLifeApp& app,
                                 const StringVec& names,
                                 const std::string& city)
{
    std::vector<tFuncLoadGpsData> funcVec;
    size_t numPerson = app.getAppSettings().getNumPerson();
    for (size_t i = 0; i < numPerson; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataCity, _1, _2,
                                         names[i], city);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

// -----------------------------------------------------------------------------

bool DataLoader::loadGpsDataYearRange(DrawingLifeApp& app,
                                      const StringVec& names,
                                      int yearStart, int yearEnd)
{
    std::vector<tFuncLoadGpsData> funcVec;

    size_t numPerson = app.getAppSettings().getNumPerson();
    for (size_t i = 0; i < numPerson; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataYearRange, _1, _2,
                                         names[i], yearStart, yearEnd);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

// -----------------------------------------------------------------------------

bool DataLoader::loadGpsDataWithSqlFile(DrawingLifeApp& app,
                                        const StringVec& sqlFilePaths)
{
    std::vector<tFuncLoadGpsData> funcVec;

    size_t numPerson = app.getAppSettings().getNumPerson();
    for (size_t i = 0; i < numPerson; ++i)
    {
        std::ifstream sqlFile(ofToDataPath(sqlFilePaths[i]).c_str(),
                              std::ifstream::in);
        std::string sqlFileSource = std::string(std::istreambuf_iterator<char>(sqlFile),
                                                std::istreambuf_iterator<char>());

        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataWithSqlFile, _1, _2,
                                         sqlFileSource);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

// -----------------------------------------------------------------------------

bool DataLoader::loadGpsData(DrawingLifeApp& app,
                             const std::vector<tFuncLoadGpsData>& funcVec)
{
    const AppSettings& settings = app.getAppSettings();
    GpsDataVector& gpsDatas = app.getGpsDataVector();
    WalkVector& walks = app.getWalkVector();
    MagicBoxVector& magicBoxes = app.getMagicBoxVector();
    ViewAspectRatioData viewAspectRatioData = app.getViewAspectRatioData();

    prepareGpsData(app);

    bool dbOk = false;

    if (settings.isMultiMode())
    {
        app.setMagicBox(new MagicBox(settings,
                                     settings.getBoundingBoxSize(),
                                     settings.getBoundingBoxPadding()));
    }
    // get GpsData from database
    size_t numPerson = settings.getNumPerson();
    for(size_t i = 0; i < numPerson; ++i)
    {
        GpsData* gpsData = new GpsData(settings);
        gpsDatas.push_back(gpsData);

        if(!settings.isMultiMode())
        {
            magicBoxes.push_back(new MagicBox(settings,
                                              settings.getBoundingBoxSize(),
                                              settings.getBoundingBoxPadding()));
        }

        Walk* walk = new Walk(settings,
                              settings.getNameColors()[i]);
        walks.push_back(walk);

        walk->setViewBounds(ofGetWidth(),
                            ofGetHeight(),
                            viewAspectRatioData.offsetX[i],
                            viewAspectRatioData.offsetY[i],
                            viewAspectRatioData.minDimension[i],
                            viewAspectRatioData.padding[i]);
        walk->reset();

        DBReaderPtr dbReader(new DBReader(settings.getDatabasePath(),
                                          settings.useSpeed()));
        if (dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------------------
            // DB query
            tFuncLoadGpsData getGpsDataFunc = funcVec.at(i);
            dbOk = getGpsDataFunc(dbReader.get(), *gpsData);

            if(dbOk)
            {
                ofLog(OF_LOG_SILENT, "--> GpsData load ok!");
                ofLog(OF_LOG_SILENT, "--> Total data: %d GpsSegments, %d GpsPoints!\n",
                      gpsData->getSegments().size(),
                      gpsData->getTotalGpsPoints());

                walk->setGpsData(gpsData);
            }
            else
            {
                ofLog(OF_LOG_SILENT, "--> No GpsData loaded!");
                break;
            }
            dbReader->closeDbConnection();
        }
        // -----------------------------------------------------------------------------

        ofLog(OF_LOG_VERBOSE, "minLon: %lf, maxLon: %lf, minLat: %lf, maxLat: %lf",
          gpsData->getMinUtmX(),
          gpsData->getMaxUtmX(),
          gpsData->getMinUtmY(),
          gpsData->getMaxUtmY());
        ofLog(OF_LOG_VERBOSE, "Central Meridian: %lf",
              gpsData->getProjectionCentralMeridian());
    }

    processGpsData(app);

    return dbOk;

}

bool DataLoader::loadCurrentPointImages(DrawingLifeApp &app)
{
    const std::vector<CurrentImageData>& imageList =
            app.getCurrentPointImageList();

    app.clearCurrentPointImages();

    for(size_t i = 0; i < imageList.size(); ++i)
    {
        ofImage* tmpImg = new ofImage();
        std::string str = imageList[i].path;
        float width = imageList[i].width;
        float height= imageList[i].height;

        if(tmpImg->loadImage(str))
        {
            tmpImg->resize(width, height);
            app.addCurrentPointImage(tmpImg);
        }
        else
        {
            app.clearCurrentPointImages();
            return false;
        }
    }
    return true;
}
