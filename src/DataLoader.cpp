#include "DataLoader.h"
#include "DrawingLifeIncludes.h"
#include "DrawingLifeApp.h"
#include "DBReader.h"

//------------------------------------------------------------------------------
// GpsData loading
//------------------------------------------------------------------------------

bool DataLoader::loadGpsDataCity(DrawingLifeApp& app,
                                 const StringVec& names,
                                 const std::string& city)
{
    std::vector<tFuncLoadGpsData> funcVec;
    const size_t numPersons = app.getAppSettings().getNumPersons();
    for (size_t i = 0; i < numPersons; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataCity, _1, _2,
                                         names[i], city);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

//------------------------------------------------------------------------------

bool DataLoader::loadGpsDataYearRange(DrawingLifeApp& app,
                                      const StringVec& names,
                                      int yearStart, int yearEnd)
{
    std::vector<tFuncLoadGpsData> funcVec;

    const size_t numPersons = app.getAppSettings().getNumPersons();
    for (size_t i = 0; i < numPersons; ++i)
    {
        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataYearRange, _1, _2,
                                         names[i], yearStart, yearEnd);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

//------------------------------------------------------------------------------

bool DataLoader::loadGpsDataWithSqlFile(DrawingLifeApp& app,
                                        const StringVec& sqlFilePaths)
{
    std::vector<tFuncLoadGpsData> funcVec;

    const size_t numPersons = app.getAppSettings().getNumPersons();
    for (size_t i = 0; i < numPersons; ++i)
    {
        std::ifstream sqlFile(ofToDataPath(sqlFilePaths[i]).c_str(),
                              std::ifstream::in);
        const std::string& sqlFileSource =
                std::string(std::istreambuf_iterator<char>(sqlFile),
                            std::istreambuf_iterator<char>());

        tFuncLoadGpsData f = boost::bind(&DBReader::getGpsDataWithSqlFile, _1, _2,
                                         sqlFileSource);
        funcVec.push_back(f);
    }

    return loadGpsData(app, funcVec);
}

//------------------------------------------------------------------------------
// Other resources
//------------------------------------------------------------------------------

bool DataLoader::loadCurrentPointImages(DrawingLifeApp &app)
{
    const std::vector<CurrentPointImageData>& imageList =
            app.getCurrentPointImageList();

    app.clearCurrentPointImages();

    for (size_t i = 0; i < imageList.size(); ++i)
    {
        ofImage* tmpImg = new ofImage();
        const std::string& str = imageList[i].path;
        const float width = imageList[i].width;
        const float height= imageList[i].height;

        if (tmpImg->loadImage(str))
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

//------------------------------------------------------------------------------

void DataLoader::loadLocationImages(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const ViewDimensionsVec& viewDimensions = app.getViewDimensionsVec();
    const MagicBoxVector boxes = app.getMagicBoxVector();

    app.clearLocationImageSources();
    app.clearLocationImageVec();

    const vector<LocationImageData>& locImgVec = settings.getLocationImageData();

    BOOST_FOREACH(const LocationImageData& locImgData, locImgVec)
    {
        boost::ptr_vector<LocationImage> locVec;

        ofImagePtr img = boost::make_shared<ofImage>();

        img->loadImage(locImgData.path);
        img->resize(locImgData.width, locImgData.height);
        if (locImgData.anchorType == 1)
        {
            img->setAnchorPercent(locImgData.anchorX, locImgData.anchorY);
        }
        else if (locImgData.anchorType == 2)
        {
            img->setAnchorPoint(locImgData.anchorX, locImgData.anchorY);
        }

        app.addLocationImageSource(img);

        for (size_t i = 0; boxes.size(); ++i)
        {
            LocationImage* lImg = new LocationImage(img, boxes[i], locImgData);
            lImg->setViewBounds(viewDimensions[i]);
            locVec.push_back(lImg);
        }

        app.addLocationImageVec(locVec);
    }
}

//------------------------------------------------------------------------------

void DataLoader::loadSoundPlayers(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();

    app.clearSoundPlayers();

    BOOST_FOREACH(const std::string& sndFilePath, settings.getSoundFiles())
    {
        ofSoundPlayer* sndPlay = new ofSoundPlayer();
        sndPlay->loadSound(sndFilePath);
        sndPlay->setLoop(false);
        app.addSoundPlayer(sndPlay);
    }

}

//------------------------------------------------------------------------------

void DataLoader::loadFonts(DrawingLifeApp& app, DrawingLifeFonts& fonts)
{
    const AppSettings& settings = app.getAppSettings();

    fonts.clear();

    const DrawingLifeFontsRaw& allFonts = settings.getFonts();
    DrawingLifeFontsRaw::const_iterator it = allFonts.begin();
    DrawingLifeFontsRaw::const_iterator itEnd = allFonts.end();
    for (; it != itEnd; ++it)
    {
        const std::string& fontId = it->first;
        const std::pair<std::string, int>& f = it->second;
        const std::string& fontName = f.first;
        const int fontSize = f.second;
        ofTrueTypeFont font;
        font.loadFont(fontName, fontSize);
        fonts[fontId] = font;
    }
}

//------------------------------------------------------------------------------
// Private GpsData loading functions
//------------------------------------------------------------------------------

void DataLoader::processGpsData(DrawingLifeApp& app)
{
    const AppSettings& settings = app.getAppSettings();
    const GpsDataVector& gpsDatas = app.getGpsDataVector();

    const ViewDimensionsVec& viewDimensions = app.getViewDimensionsVec();
    const std::vector<CurrentPointImageData>& imageList =
        app.getCurrentPointImageList();
    const boost::ptr_vector<ofImage>& images = app.getCurrentPointImages();

    MagicBoxVector& magicBoxes = app.getMagicBoxVector();
    Timeline& timeline = app.getTimeline();
    WalkVector& walks = app.getWalkVector();

    ofLogVerbose(Logger::DATA_LOADER) << "------------------------\n";

    const size_t numPersons = settings.getNumPersons();

    if (numPersons > 0 && gpsDatas.size() == numPersons)
    {
        timeline.setData(gpsDatas);

        Walk::setTrackAlpha(settings.getAlphaDot());
        Walk::setDotSize(settings.getDotSize());

        for (size_t i = 0; i < numPersons; ++i)
        {
            if (!settings.isMultiMode() || magicBoxes.empty())
            {
                magicBoxes.push_back(
                    boost::make_shared<MagicBox>(settings,
                                                 settings.getBoundingBoxSize(),
                                                 settings.getBoundingBoxPadding()));
            }

            MagicBoxWeak boxWeak = magicBoxes.back();

            Walk* walk = new Walk(settings, settings.getNameColors()[i]);
            walks.push_back(walk);

            walk->setViewBounds(viewDimensions[i]);
            walk->reset();

            walk->setGpsData(gpsDatas[i]->shared_from_this());

            if (settings.isBoundingBoxCropMode())
            {
                walk->setMagicBox(boxWeak);
            }
            else
            {
                double bbLat = settings.getBoundingBoxLat();
                double bbLon = settings.getBoundingBoxLon();
                walk->setMagicBoxStatic(boxWeak, bbLat, bbLon);
            }

            if (app.getIsImageAsCurrentPoint() &&
                images.size() >= settings.getNumPersons())
            {
                walk->setCurrentPointImage(images[i], imageList[i].alpha);
            }
        }
    }
}

//------------------------------------------------------------------------------

bool DataLoader::loadGpsData(DrawingLifeApp& app,
                             const std::vector<tFuncLoadGpsData>& funcVec)
{
    const AppSettings& settings = app.getAppSettings();
    GpsDataVector& gpsDatas = app.getGpsDataVector();

    app.resetData();

    // get GpsData from database
    const size_t numPersons = settings.getNumPersons();
    for (size_t i = 0; i < numPersons; ++i)
    {
        GpsDataPtr gpsData = boost::make_shared<GpsData>(settings);
        gpsDatas.push_back(gpsData);

        DBReaderPtr dbReader(new DBReader(settings.getDatabasePath(),
                                          settings.useSpeed()));
        if (dbReader->setupDbConnection())
        {
            // -----------------------------------------------------------------
            // DB query
            tFuncLoadGpsData getGpsDataFunc = funcVec.at(i);
            const bool loadOk = getGpsDataFunc(dbReader.get(), *gpsData);

            dbReader->closeDbConnection();

            if (loadOk)
            {
                ofLogNotice(Logger::DATA_LOADER) << "--> GpsData load ok!";
                ofLogNotice(Logger::DATA_LOADER)
                        << "--> Total data: "
                        << gpsData->getSegments().size() << " GpsSegments, "
                        << gpsData->getTotalGpsPoints() << " GpsPoints!"
                        << std::endl;
            }
            else
            {
                ofLogNotice(Logger::DATA_LOADER) << "--> No GpsData loaded!";
                return false;
            }
        }
        // ---------------------------------------------------------------------

        ofLogVerbose(Logger::DATA_LOADER)
                << "minLon: " << gpsData->getMinUtmX() << ", "
                << "maxLon: " << gpsData->getMaxUtmX() << ", "
                << "minLat: " << gpsData->getMinUtmY() << ", "
                << "maxLat: " << gpsData->getMaxUtmY();
    }

    processGpsData(app);

    return true;

}

//------------------------------------------------------------------------------
